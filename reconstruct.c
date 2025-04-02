#include <stdio.h>
#include <stdlib.h>

int main() {
    int tid, nxglob, nyglob, px, py, nx, ny, i, j, rank, rank_x, rank_y, istglob, ienglob, jstglob, jenglob;
    int i_local, j_local;

    printf("Enter the time step ID: ");
    scanf("%d", &tid);

    FILE* fid = fopen("input2d.in", "r");
    if (!fid) {
        printf("Error opening input file\n");
        return 1;
    }

    double xstglob, xenglob, ystglob, yenglob, dx, dy;
    fscanf(fid, "%d %d\n", &nxglob, &nyglob);
    fscanf(fid, "%lf %lf %lf %lf\n", &xstglob, &xenglob, &ystglob, &yenglob);
    fscanf(fid, "%*lf %*lf %*lf %*lf\n"); // Skip other inputs
    fscanf(fid, "%*lf\n"); // Skip kdiff
    fscanf(fid, "%d %d\n", &px, &py);
    fclose(fid);

    // Compute global x and y coordinates
    dx = (xenglob - xstglob) / (nxglob - 1);
    dy = (yenglob - ystglob) / (nyglob - 1);
    double* x = malloc(nxglob * sizeof(double));
    double* y = malloc(nyglob * sizeof(double));
    for (i = 0; i < nxglob; i++) x[i] = xstglob + i * dx;
    for (j = 0; j < nyglob; j++) y[j] = ystglob + j * dy;

    double** Tglob = malloc(nxglob * sizeof(double*));
    for (i = 0; i < nxglob; i++) Tglob[i] = calloc(nyglob, sizeof(double));

    nx = nxglob / px;
    ny = nyglob / py;

    for (rank = 0; rank < px * py; rank++) {
        rank_y = rank / px;
        rank_x = rank % px;

        istglob = rank_x * nx;
        ienglob = (rank_x + 1) * nx - 1;
        jstglob = rank_y * ny;
        jenglob = (rank_y + 1) * ny - 1;

        char fname[100];
        sprintf(fname, "T_x_y_%06d_%04d.dat", tid, rank);
        fid = fopen(fname, "r");
        if (!fid) {
            printf("Error opening %s\n", fname);
            return 1;
        }

        // Read local T values and map to global Tglob
        for (i_local = 0; i_local < nx; i_local++) {
            for (j_local = 0; j_local < ny; j_local++) {
                double x_val, y_val, T_val;
                fscanf(fid, "%lf %lf %lf\n", &x_val, &y_val, &T_val);
                int i_glob = istglob + i_local;
                int j_glob = jstglob + j_local;
                Tglob[i_glob][j_glob] = T_val;
            }
        }
        fclose(fid);
    }

    char new_fname[100];
    sprintf(new_fname, "T_x_y_par_%06d.dat", tid);
    fid = fopen(new_fname, "w");
    for (i = 0; i < nxglob; i++) {
        for (j = 0; j < nyglob; j++) {
            fprintf(fid, "%lf %lf %lf\n", x[i], y[j], Tglob[i][j]);
        }
    }
    fclose(fid);
    printf("Done writing %s\n", new_fname);

    // Free memory
    free(x);
    free(y);
    for (i = 0; i < nxglob; i++) free(Tglob[i]);
    free(Tglob);
    return 0;
}