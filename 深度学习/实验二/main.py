import warnings
from numpy.exceptions import VisibleDeprecationWarning

warnings.filterwarnings(
    "ignore",
    message=".*align should be passed as Python or NumPy boolean.*",
    category=VisibleDeprecationWarning
)

import argparse
import os
import random
import time

import numpy as np
import torch
from torch import nn, optim
from torch.utils.data import DataLoader
from torchvision import datasets, transforms

from resnet import ResNet13


# CIFAR10 推荐的归一化参数，不建议继续用 ImageNet 的 mean/std。
CIFAR10_MEAN = (0.4914, 0.4822, 0.4465)
CIFAR10_STD = (0.2470, 0.2435, 0.2616)


def set_seed(seed: int = 42):
    random.seed(seed)
    np.random.seed(seed)
    torch.manual_seed(seed)

    if torch.cuda.is_available():
        torch.cuda.manual_seed_all(seed)


def get_device():
    """优先使用 NVIDIA CUDA GPU；如果不可用则自动回退 CPU。"""
    if torch.cuda.is_available():
        device = torch.device('cuda:0')

        torch.backends.cudnn.benchmark = True
        torch.backends.cuda.matmul.allow_tf32 = True
        torch.backends.cudnn.allow_tf32 = True

        try:
            torch.set_float32_matmul_precision('high')
        except Exception:
            pass

        print(f'Using CUDA GPU: {torch.cuda.get_device_name(0)}')
    else:
        device = torch.device('cpu')
        print('CUDA GPU not available, using CPU.')

    return device


def build_dataloader(dataset, batch_size, shuffle, use_cuda):
    # Windows 下 workers 太高有时会拖慢或报错，4 通常比较稳。
    num_workers = min(4, os.cpu_count() or 1) if use_cuda else 0

    kwargs = dict(
        batch_size=batch_size,
        shuffle=shuffle,
        num_workers=num_workers,
        pin_memory=use_cuda,
    )

    if num_workers > 0:
        kwargs.update(
            persistent_workers=True,
            prefetch_factor=4,
        )

    return DataLoader(dataset, **kwargs)


def move_batch_to_device(x, label, device, use_cuda):
    x = x.to(device, non_blocking=use_cuda)
    label = label.to(device, non_blocking=use_cuda)

    if use_cuda:
        x = x.contiguous(memory_format=torch.channels_last)

    return x, label


def accuracy_from_logits(logits, labels):
    pred = logits.argmax(dim=1)
    return pred.eq(labels).sum().item()


def main():
    parser = argparse.ArgumentParser(description='Train ResNet on CIFAR10 with CUDA acceleration.')
    epoch1=20
    parser.add_argument('--data-dir', type=str, default='cifar-10-python')
    parser.add_argument('--epochs', type=int, default=epoch1)
    parser.add_argument('--batch-size', type=int, default=512)
    parser.add_argument('--lr', type=float, default=0.1)
    parser.add_argument('--weight-decay', type=float, default=5e-4)
    parser.add_argument('--seed', type=int, default=42)
    parser.add_argument('--test-every', type=int, default=epoch1)
    parser.add_argument('--amp', action='store_true', default=True)
    parser.add_argument('--no-amp', action='store_false', dest='amp')
    args = parser.parse_args()

    set_seed(args.seed)

    device = get_device()
    use_cuda = device.type == 'cuda'
    use_amp = use_cuda and args.amp

    train_transform = transforms.Compose([
        transforms.RandomCrop(32, padding=4),
        transforms.RandomHorizontalFlip(),
        transforms.ToTensor(),
        transforms.Normalize(CIFAR10_MEAN, CIFAR10_STD),
    ])

    test_transform = transforms.Compose([
        transforms.ToTensor(),
        transforms.Normalize(CIFAR10_MEAN, CIFAR10_STD),
    ])

    train_set = datasets.CIFAR10(
        args.data_dir,
        train=True,
        transform=train_transform,
        download=False,
    )

    test_set = datasets.CIFAR10(
        args.data_dir,
        train=False,
        transform=test_transform,
        download=False,
    )

    train_loader = build_dataloader(
        train_set,
        args.batch_size,
        shuffle=True,
        use_cuda=use_cuda,
    )

    test_loader = build_dataloader(
        test_set,
        args.batch_size,
        shuffle=False,
        use_cuda=use_cuda,
    )

    model = ResNet13(num_classes=10).to(device)

    if use_cuda:
        model = model.to(memory_format=torch.channels_last)

    # 准确率优先：SGD + momentum + weight_decay + Cosine LR 通常比 Adam 更适合 CIFAR10 ResNet。
    criterion = nn.CrossEntropyLoss(label_smoothing=0.1).to(device)

    optimizer = optim.SGD(
        model.parameters(),
        lr=args.lr,
        momentum=0.9,
        weight_decay=args.weight_decay,
        nesterov=True,
    )

    scheduler = optim.lr_scheduler.CosineAnnealingLR(
        optimizer,
        T_max=args.epochs,
    )

    scaler = torch.amp.GradScaler('cuda', enabled=use_amp)

    print(
        f'batch_size={args.batch_size}, epochs={args.epochs}, lr={args.lr}, '
        f'weight_decay={args.weight_decay}, amp={use_amp}, test_every={args.test_every}'
    )

    print(f'train_size={len(train_set)}, test_size={len(test_set)}')

    best_acc = 0.0
    best_path = 'cifar10.pth'

    for epoch in range(1, args.epochs + 1):
        start = time.perf_counter()

        model.train()

        running_loss = 0.0
        train_correct = 0
        train_total = 0

        for x, label in train_loader:
            x, label = move_batch_to_device(x, label, device, use_cuda)

            optimizer.zero_grad(set_to_none=True)

            with torch.amp.autocast('cuda', enabled=use_amp):
                logits = model(x)
                loss = criterion(logits, label)

            scaler.scale(loss).backward()
            scaler.step(optimizer)
            scaler.update()

            running_loss += loss.item() * x.size(0)
            train_correct += accuracy_from_logits(logits.detach(), label)
            train_total += x.size(0)

        scheduler.step()

        train_loss = running_loss / train_total
        train_acc = train_correct / train_total
        elapsed = time.perf_counter() - start
        lr_now = scheduler.get_last_lr()[0]

        print(
            f'epoch {epoch:03d}/{args.epochs} | '
            f'loss: {train_loss:.4f} | train acc: {train_acc:.4f} | '
            f'lr: {lr_now:.5f} | train_time: {elapsed:.2f}s'
        )

        # 训练结束后 test 一次
        if epoch % args.test_every == 0:
            model.eval()

            test_correct = 0
            test_total = 0

            with torch.no_grad():
                for x, label in test_loader:
                    x, label = move_batch_to_device(x, label, device, use_cuda)

                    with torch.amp.autocast('cuda', enabled=use_amp):
                        logits = model(x)

                    test_correct += accuracy_from_logits(logits, label)
                    test_total += x.size(0)

            test_acc = test_correct / test_total
            improved = test_acc > best_acc

            if improved:
                best_acc = test_acc

                torch.save(
                    {
                        'epoch': epoch,
                        'model_state_dict': model.state_dict(),
                        'optimizer_state_dict': optimizer.state_dict(),
                        'scheduler_state_dict': scheduler.state_dict(),
                        'best_acc': best_acc,
                        'args': vars(args),
                    },
                    best_path,
                )

            mark = '  <-- best' if improved else ''

            print(
                f'epoch {epoch:03d}/{args.epochs} | '
                f'test acc: {test_acc:.4f} | best acc: {best_acc:.4f}{mark}'
            )

    if use_cuda:
        print(f'max GPU memory allocated: {torch.cuda.max_memory_allocated() / 1024 ** 3:.2f} GB')

    print(f'best model saved to: {best_path}')


if __name__ == '__main__':
    main()