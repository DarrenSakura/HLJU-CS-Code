import torch
from torch import nn
from torch.nn import functional as F

class ResBlk(nn.Module):

    def __init__(self, ch_in, ch_out, stride=1):
        super().__init__()

        self.conv1 = nn.Conv2d(ch_in, ch_out, kernel_size=3, stride=stride, padding=1, bias=False)
        self.bn1 = nn.BatchNorm2d(ch_out)
        self.conv2 = nn.Conv2d(ch_out, ch_out, kernel_size=3, stride=1, padding=1, bias=False)
        self.bn2 = nn.BatchNorm2d(ch_out)

        self.shortcut = nn.Identity()
        if stride != 1 or ch_in != ch_out:
            self.shortcut = nn.Sequential(
                nn.Conv2d(ch_in, ch_out, kernel_size=1, stride=stride, bias=False),
                nn.BatchNorm2d(ch_out),
            )

    def forward(self, x):
        out = F.relu(self.bn1(self.conv1(x)), inplace=True)
        out = self.bn2(self.conv2(out))
        out = out + self.shortcut(x)
        out = F.relu(out, inplace=True)
        return out

class ResNet13(nn.Module):

    def __init__(self, num_classes=10):
        super().__init__()

        self.stem = nn.Sequential(
            nn.Conv2d(3, 64, kernel_size=3, stride=1, padding=1, bias=False),
            nn.BatchNorm2d(64),
            nn.ReLU(inplace=True),
        )

        self.layer1 = nn.Sequential(
            ResBlk(64, 64, stride=1),
            ResBlk(64, 64, stride=1),
        )
        self.layer2 = nn.Sequential(
            ResBlk(64, 128, stride=2),
            ResBlk(128, 128, stride=1),
        )
        self.layer3 = nn.Sequential(
            ResBlk(128, 256, stride=2),
            ResBlk(256, 256, stride=1),
        )
        self.layer4 = nn.Sequential(
            ResBlk(256, 512, stride=2),
            ResBlk(512, 512, stride=1),
        )

        self.pool = nn.AdaptiveAvgPool2d(1)
        self.fc = nn.Linear(512, num_classes)

        self._init_weights()

    def _init_weights(self):
        for m in self.modules():
            if isinstance(m, nn.Conv2d):
                nn.init.kaiming_normal_(m.weight, mode='fan_out', nonlinearity='relu')
            elif isinstance(m, nn.BatchNorm2d):
                nn.init.ones_(m.weight)
                nn.init.zeros_(m.bias)
            elif isinstance(m, nn.Linear):
                nn.init.normal_(m.weight, 0, 0.01)
                nn.init.zeros_(m.bias)

    def forward(self, x):
        x = self.stem(x)
        x = self.layer1(x)
        x = self.layer2(x)
        x = self.layer3(x)
        x = self.layer4(x)
        x = self.pool(x)
        x = torch.flatten(x, 1)
        x = self.fc(x)
        return x

def get_device():
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

def main():
    device = get_device()
    use_cuda = device.type == 'cuda'

    model = ResNet13().to(device)
    if use_cuda:
        model = model.to(memory_format=torch.channels_last)

    x = torch.randn(2, 3, 32, 32, device=device)
    if use_cuda:
        x = x.contiguous(memory_format=torch.channels_last)

    out = model(x)
    print('resnet:', out.shape, 'device:', out.device)

if __name__ == '__main__':
    main()