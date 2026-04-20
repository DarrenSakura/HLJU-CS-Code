import  torch
import  os
from    torch.utils.data import DataLoader
from    torchvision import datasets
from    torchvision import transforms
from    torch import nn, optim

from    resnet import ResNet13

def main():
    batchsz = 512
    epochs = 10
    eval_interval = 5
    num_workers = min(8, os.cpu_count() or 1)

    cifar_train = datasets.CIFAR10('cifar', True, transform=transforms.Compose([
        transforms.Resize((32, 32)),
        transforms.ToTensor(),
        transforms.Normalize(mean=[0.485, 0.456, 0.406],
                             std=[0.229, 0.224, 0.225])
    ]), download=True)
    cifar_train = DataLoader(
        cifar_train,
        batch_size=batchsz,
        shuffle=True,
        num_workers=num_workers,
        persistent_workers=num_workers > 0
    )

    cifar_test = datasets.CIFAR10('cifar', False, transform=transforms.Compose([
        transforms.Resize((32, 32)),
        transforms.ToTensor(),
        transforms.Normalize(mean=[0.485, 0.456, 0.406],
                             std=[0.229, 0.224, 0.225])
    ]), download=True)
    cifar_test = DataLoader(
        cifar_test,
        batch_size=batchsz,
        shuffle=False,
        num_workers=num_workers,
        persistent_workers=num_workers > 0
    )


    x, label = next(iter(cifar_train))
    print('x:', x.shape, 'label:', label.shape)

    if hasattr(torch.backends, 'mps') and torch.backends.mps.is_available():
        device = torch.device('mps')
    elif torch.cuda.is_available():
        device = torch.device('cuda')
    else:
        device = torch.device('cpu')
    print('using device:', device, 'num_workers:', num_workers)
    model = ResNet13().to(device)

    criteon = nn.CrossEntropyLoss().to(device)
    optimizer = optim.Adam(model.parameters(), lr=1e-3)
    print(model)

    for epoch in range(epochs):

        model.train()
        for batchidx, (x, label) in enumerate(cifar_train):
            # [b, 3, 32, 32]
            # [b]
            x, label = x.to(device, non_blocking=True), label.to(device, non_blocking=True)


            logits = model(x)
            # logits: [b, 10]
            # label:  [b]
            # loss: tensor scalar
            loss = criteon(logits, label)

            # backprop
            optimizer.zero_grad()
            loss.backward()
            optimizer.step()


        print(epoch, 'loss:', loss.item())

        if (epoch + 1) % eval_interval != 0:
            continue


        model.eval()
        with torch.no_grad():
            # test
            total_correct = 0
            total_num = 0
            for x, label in cifar_test:
                # [b, 3, 32, 32]
                # [b]
                x, label = x.to(device, non_blocking=True), label.to(device, non_blocking=True)

                # [b, 10]
                logits = model(x)
                # [b]
                pred = logits.argmax(dim=1)
                # [b] vs [b] => scalar tensor
                correct = torch.eq(pred, label).float().sum().item()
                total_correct += correct
                total_num += x.size(0)
                # print(correct)

            acc = total_correct / total_num
            print(epoch, 'test acc:', acc)



if __name__ == '__main__':
    main()
