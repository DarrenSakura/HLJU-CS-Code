import os.path as osp

import torch
import torch.nn.functional as F
import torch_geometric.transforms as T
from torch_geometric.datasets import Planetoid
from torch_geometric.nn import GATConv, GCNConv, SAGEConv, SplineConv

SEED = 42
EPOCHS = 200
HIDDEN = 16

torch.manual_seed(SEED)

dataset_name = 'Cora'
path = osp.join(osp.dirname(osp.realpath(__file__)), '..', 'data', dataset_name)
dataset = Planetoid(path, dataset_name, transform=T.TargetIndegree())
data = dataset[0]

data.train_mask = torch.zeros(data.num_nodes, dtype=torch.bool)
data.train_mask[:data.num_nodes - 1000] = 1
data.val_mask = None
data.test_mask = torch.zeros(data.num_nodes, dtype=torch.bool)
data.test_mask[data.num_nodes - 500:] = 1


class SplineNet(torch.nn.Module):
    def __init__(self):
        super().__init__()
        self.conv1 = SplineConv(dataset.num_features, HIDDEN, dim=1, kernel_size=2)
        self.conv2 = SplineConv(HIDDEN, dataset.num_classes, dim=1, kernel_size=2)

    def forward(self, graph_data):
        x, edge_index, edge_attr = graph_data.x, graph_data.edge_index, graph_data.edge_attr
        x = F.dropout(x, training=self.training)
        x = F.elu(self.conv1(x, edge_index, edge_attr))
        x = F.dropout(x, training=self.training)
        x = self.conv2(x, edge_index, edge_attr)
        return F.log_softmax(x, dim=1)


class GCNNet(torch.nn.Module):
    def __init__(self):
        super().__init__()
        self.conv1 = GCNConv(dataset.num_features, HIDDEN)
        self.conv2 = GCNConv(HIDDEN, dataset.num_classes)

    def forward(self, graph_data):
        x, edge_index = graph_data.x, graph_data.edge_index
        x = F.dropout(x, training=self.training)
        x = F.relu(self.conv1(x, edge_index))
        x = F.dropout(x, training=self.training)
        x = self.conv2(x, edge_index)
        return F.log_softmax(x, dim=1)


class GraphSAGENet(torch.nn.Module):
    def __init__(self):
        super().__init__()
        self.conv1 = SAGEConv(dataset.num_features, HIDDEN)
        self.conv2 = SAGEConv(HIDDEN, dataset.num_classes)

    def forward(self, graph_data):
        x, edge_index = graph_data.x, graph_data.edge_index
        x = F.dropout(x, training=self.training)
        x = F.relu(self.conv1(x, edge_index))
        x = F.dropout(x, training=self.training)
        x = self.conv2(x, edge_index)
        return F.log_softmax(x, dim=1)


class GATNet(torch.nn.Module):
    def __init__(self):
        super().__init__()
        self.conv1 = GATConv(dataset.num_features, 8, heads=8, dropout=0.6)
        self.conv2 = GATConv(8 * 8, dataset.num_classes, heads=1, concat=False, dropout=0.6)

    def forward(self, graph_data):
        x, edge_index = graph_data.x, graph_data.edge_index
        x = F.dropout(x, p=0.6, training=self.training)
        x = F.elu(self.conv1(x, edge_index))
        x = F.dropout(x, p=0.6, training=self.training)
        x = self.conv2(x, edge_index)
        return F.log_softmax(x, dim=1)


def get_device():
    if torch.backends.mps.is_available():
        return torch.device('mps')
    if torch.cuda.is_available():
        return torch.device('cuda')
    return torch.device('cpu')


def train_one_epoch(model, optimizer, graph_data):
    model.train()
    optimizer.zero_grad()
    out = model(graph_data)
    loss = F.nll_loss(out[graph_data.train_mask], graph_data.y[graph_data.train_mask])
    loss.backward()
    optimizer.step()
    return loss.item()


@torch.no_grad()
def evaluate(model, graph_data):
    model.eval()
    log_probs = model(graph_data)
    accs = []
    for _, mask in graph_data('train_mask', 'test_mask'):
        pred = log_probs[mask].max(1)[1]
        acc = pred.eq(graph_data.y[mask]).sum().item() / mask.sum().item()
        accs.append(acc)
    return accs


def run_experiment(model_name, model_cls, graph_data, device):
    model = model_cls().to(device)
    optimizer = torch.optim.Adam(model.parameters(), lr=0.01, weight_decay=5e-3)

    best_test = 0.0
    final_train = 0.0
    final_test = 0.0

    print(f'\n===== {model_name} =====')
    for epoch in range(1, EPOCHS + 1):
        loss = train_one_epoch(model, optimizer, graph_data)
        train_acc, test_acc = evaluate(model, graph_data)
        best_test = max(best_test, test_acc)
        final_train, final_test = train_acc, test_acc

        if epoch % 20 == 0 or epoch == 1:
            print(
                f'Epoch: {epoch:03d}, Loss: {loss:.4f}, '
                f'Train: {train_acc:.4f}, Test: {test_acc:.4f}, Best: {best_test:.4f}'
            )

    return {
        'model': model_name,
        'final_train_acc': final_train,
        'final_test_acc': final_test,
        'best_test_acc': best_test,
    }


def main():
    device = torch.device("cpu")
    graph_data = data.to(device)
    print(f'Device: {device}')

    model_zoo = {
        # 'SplineConv': SplineNet,
        'GCNConv': GCNNet,
        'SAGEConv': GraphSAGENet,
        'GATConv': GATNet,
    }

    results = []
    for model_name, model_cls in model_zoo.items():
        torch.manual_seed(SEED)
        results.append(run_experiment(model_name, model_cls, graph_data, device))

    print('\n===== Summary (Cora) =====')
    print('Model       FinalTrain   FinalTest   BestTest')
    for r in results:
        print(
            f"{r['model']:<11} {r['final_train_acc']:.4f}      "
            f"{r['final_test_acc']:.4f}     {r['best_test_acc']:.4f}"
        )

    best_model = max(results, key=lambda x: x['best_test_acc'])
    print(
        f"\nBest model on test set: {best_model['model']} "
        f"(BestTest={best_model['best_test_acc']:.4f})"
    )


if __name__ == '__main__':
    main()
