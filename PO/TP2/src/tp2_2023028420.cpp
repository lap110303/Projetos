#include <iostream>
#include <iomanip>
#include <cmath>
#include <limits>

const double EPS = 1e-9;

int main(int argc, char* argv[]) {
    
    int n, m;
    std::cin >> n >> m;

    double *c = new double[m];
    for (int j = 0; j < m; ++j) std::cin >> c[j];

    double **A = new double*[n];
    double *b = new double[n];
    for (int i = 0; i < n; ++i) {
        A[i] = new double[m];
        for (int j = 0; j < m; ++j) std::cin >> A[i][j];
        std::cin >> b[i];
    }

    int rows = n + 1;
    int cols = m + n + 1;
    double **T = new double*[rows];
    for (int i = 0; i < rows; ++i) {
        T[i] = new double[cols];
        for (int j = 0; j < cols; ++j) T[i][j] = 0.0;
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) T[i][j] = A[i][j];
        T[i][m + i] = 1.0;       
        T[i][cols-1] = b[i];     
    }

    for (int j = m; j < m + n; ++j) T[n][j] = -1.0;
    T[n][cols-1] = 0.0;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < cols; ++j) T[n][j] += T[i][j];
    }


    int *basis = new int[n];
    for (int i = 0; i < n; ++i) basis[i] = m + i;

    auto pivot = [&](int pr, int pc) {
        double inv = 1.0 / T[pr][pc];
        for (int j = 0; j < cols; ++j) T[pr][j] *= inv;
        for (int i = 0; i < rows; ++i) {
            if (i == pr) continue;
            double f = T[i][pc];
            for (int j = 0; j < cols; ++j) T[i][j] -= f * T[pr][j];
        }
        basis[pr] = pc;
    };


    while (true) {
        int pc = -1;
        for (int j = 0; j < cols-1; ++j) if (T[n][j] > EPS) { pc = j; break; }
        if (pc == -1) break;
        int pr = -1;
        double best = 1e300;
        for (int i = 0; i < n; ++i) {
            if (T[i][pc] > EPS) {
                double ratio = T[i][cols-1] / T[i][pc];
                if (ratio < best) { best = ratio; pr = i; }
            }
        }
        if (pr == -1) { std::cout << "inviavel"; return 0; }
        pivot(pr, pc);
    }

    if (std::fabs(T[n][cols-1]) > EPS) {
        std::cout << "inviavel";
        return 0;
    }


    for (int i = 0; i < n; ++i) {
        if (basis[i] >= m) {
            for (int j = 0; j < m; ++j) {
                if (std::fabs(T[i][j]) > EPS) { pivot(i, j); break; }
            }
        }
    }

    for (int j = 0; j < cols; ++j) T[n][j] = 0.0;
    for (int j = 0; j < m; ++j) T[n][j] = -c[j];
    T[n][cols-1] = 0.0;

    for (int i = 0; i < n; ++i) {
        int bi = basis[i];
        if (bi < m) {
            double coef = T[n][bi];
            if (std::fabs(coef) > EPS) {
                for (int j = 0; j < cols; ++j) T[n][j] -= coef * T[i][j];
            }
        }
    }

    while (true) {
        int pc = -1;
        for (int j = 0; j < m; ++j) if (T[n][j] < -EPS) { pc = j; break; }
        if (pc == -1) break;
        int pr = -1;
        double best = 1e300;
        for (int i = 0; i < n; ++i) {
            if (T[i][pc] > EPS) {
                double ratio = T[i][cols-1] / T[i][pc];
                if (ratio < best) { best = ratio; pr = i; }
            }
        }
        if (pr == -1) { std::cout << "ilimitada"; return 0; }
        pivot(pr, pc);
    }

    double opt = T[n][cols-1];
    std::cout << "otima" << std::endl;
    std::cout << std::fixed << std::setprecision(3) << opt;
    return 0;
}
