package exp5;

public class Matrix {
    private double[][] data;
    private int rows;
    private int cols;

    public Matrix(int rows, int cols) throws IllegalArgumentException {
        if (rows <= 0 || cols <= 0) {
            throw new IllegalArgumentException();
        }
        this.rows = rows;
        this.cols = cols;
        this.data = new double[rows][cols];
    }

    public Matrix(double[][] data) throws IllegalArgumentException {
        if (data == null || data.length == 0 || data[0].length == 0) {
            throw new IllegalArgumentException();
        }
        this.rows = data.length;
        this.cols = data[0].length;
        this.data = new double[rows][cols];
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                this.data[i][j] = data[i][j];
            }
        }
    }

    public double getData(int row, int col) throws IllegalIndexException {
        if (row < 0 || row >= rows || col < 0 || col >= cols) {
            throw new IllegalIndexException();
        }
        return data[row][col];
    }

    public void setData(int row, int col, double value) throws IllegalIndexException {
        if (row < 0 || row >= rows || col < 0 || col >= cols) {
            throw new IllegalIndexException();
        }
        data[row][col] = value;
    }

    public int getRows() {
        return rows;
    }

    public int getCols() {
        return cols;
    }

    public Matrix multiply(Matrix m) throws MatrixMultiplicationException, IllegalArgumentException, IllegalIndexException {
        if (this.cols != m.getRows()) {
            throw new MatrixMultiplicationException();
        }
        Matrix result = new Matrix(this.rows, m.getCols());
        for (int i = 0; i < this.rows; i++) {
            for (int j = 0; j < m.getCols(); j++) {
                double sum = 0;
                for (int k = 0; k < this.cols; k++) {
                    sum += this.data[i][k] * m.getData(k, j);
                }
                result.setData(i, j, sum);
            }
        }
        return result;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        Matrix matrix = (Matrix) obj;
        if (rows != matrix.rows || cols != matrix.cols) return false;
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                if (Double.compare(matrix.data[i][j], data[i][j]) != 0) {
                    return false;
                }
            }
        }
        return true;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                sb.append(data[i][j]).append("\t");
            }
            sb.append("\n");
        }
        return sb.toString();
    }
}