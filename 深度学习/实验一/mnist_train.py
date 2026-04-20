import  torch
from    torch import nn
from    torch.nn import functional as F
from    torch import optim

import  torchvision
from    matplotlib import pyplot as plt

from    utils import plot_image, plot_curve



batch_size = 128
epochs = 5

torch.manual_seed(42)

device = torch.device('mps' if torch.backends.mps.is_available() else 'cpu')
print('using device:', device)

# step1. load dataset
train_loader = torch.utils.data.DataLoader(
    torchvision.datasets.MNIST('mnist_data', train=True, download=True,
                               transform=torchvision.transforms.Compose([
                                   torchvision.transforms.ToTensor(),
                                   torchvision.transforms.Normalize(
                                       (0.1307,), (0.3081,))
                               ])),
    batch_size=batch_size, shuffle=True)

test_loader = torch.utils.data.DataLoader(
    torchvision.datasets.MNIST('mnist_data/', train=False, download=True,
                               transform=torchvision.transforms.Compose([
                                   torchvision.transforms.ToTensor(),
                                   torchvision.transforms.Normalize(
                                       (0.1307,), (0.3081,))
                               ])),
    batch_size=batch_size, shuffle=False)

x, y = next(iter(train_loader))
print(x.shape, y.shape, x.min(), x.max())
plot_image(x, y, 'image sample')



class Net(nn.Module):

    def __init__(self):
        super(Net, self).__init__()

        # xw+b
        self.fc1 = nn.Linear(28*28, 512)
        self.fc2 = nn.Linear(512, 256)
        self.fc3 = nn.Linear(256, 10)
        self.dropout = nn.Dropout(0.2)

    def forward(self, x):
        # x: [b, 1, 28, 28]
        # h1 = relu(xw1+b1)
        x = F.relu(self.fc1(x))
        x = self.dropout(x)
        # h2 = relu(h1w2+b2)
        x = F.relu(self.fc2(x))
        x = self.dropout(x)
        # h3 = h2w3+b3
        x = self.fc3(x)

        return x



net = Net().to(device)
# [w1, b1, w2, b2, w3, b3]
optimizer = optim.Adam(net.parameters(), lr=1e-3)


train_loss = []

for epoch in range(epochs):

    for batch_idx, (x, y) in enumerate(train_loader):

        # x: [b, 1, 28, 28], y: [512]
        x, y = x.to(device), y.to(device)
        # [b, 1, 28, 28] => [b, 784]
        x = x.view(x.size(0), 28*28)
        # => [b, 10]
        out = net(x)
        loss = F.cross_entropy(out, y)

        optimizer.zero_grad()
        loss.backward()
        # w' = w - lr*grad
        optimizer.step()

        train_loss.append(loss.item())

        if batch_idx % 10==0:
            print(epoch, batch_idx, loss.item())

plot_curve(train_loss)
# we get optimal [w1, b1, w2, b2, w3, b3]


total_correct = 0
net.eval()
for x,y in test_loader:
    x, y = x.to(device), y.to(device)
    x  = x.view(x.size(0), 28*28)
    out = net(x)
    # out: [b, 10] => pred: [b]
    pred = out.argmax(dim=1)
    correct = pred.eq(y).sum().float().item()
    total_correct += correct

total_num = len(test_loader.dataset)
acc = total_correct / total_num
print('test acc:', acc)

x, y = next(iter(test_loader))
out = net(x.to(device).view(x.size(0), 28*28))
pred = out.argmax(dim=1)
plot_image(x, pred.cpu(), 'test')





