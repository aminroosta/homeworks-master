/*
	minimize
		z = 20*x1 + 15*x2
	subject to
		p = .4*x1 + .32*x2
		q = .2*x1 + .4*x2
		r = .35*x1 + .2*x2
	and bounds of variables
		p >= 1000*1000
		q >= 500*1000
		r >= 300*1000
		x1, x2 >= 0 (both integers).
*/

#include <stdio.h>
#include <stdlib.h>
#include <glpk.h>

int main(void) {
	glp_prob *lp;
	int ia[1+1000], ja[1+1000];
	double ar[1+1000], z, x1, x2;
	lp = glp_create_prob();
	glp_set_prob_name(lp, "homework 1 - question 1");
	glp_set_obj_dir(lp, GLP_MIN);

	glp_add_rows(lp, 3);
	glp_set_row_name(lp, 1, "p");
	glp_set_row_bnds(lp, 1, GLP_LO, 1000*1000.0, 0.0);
	glp_set_row_name(lp, 2, "q");
	glp_set_row_bnds(lp, 2, GLP_LO, 500*1000.0, 0.0);
	glp_set_row_name(lp, 3, "r");
	glp_set_row_bnds(lp, 3, GLP_LO, 300*1000.0, 0.0);

	glp_add_cols(lp, 2);
	glp_set_col_name(lp, 1, "x1");
	glp_set_col_bnds(lp, 1, GLP_LO, 0.0, 0.0);
	glp_set_obj_coef(lp, 1, 20.0);
	glp_set_col_kind(lp, 1, GLP_IV);
	glp_set_col_name(lp, 2, "x2");
	glp_set_col_bnds(lp, 2, GLP_LO, 0.0, 0.0);
	glp_set_obj_coef(lp, 2, 15.0);
	glp_set_col_kind(lp, 2, GLP_IV);
		// p = .4*x1 + .32*x2
		// q = .2*x1 + .4*x2
		// r = .35*x1 + .2*x2

	ia[1] = 1, ja[1] = 1, ar[1] =  .4; /* a[1,1] =  .4 */
	ia[2] = 1, ja[2] = 2, ar[2] =  .32; /* a[1,2] =  .32 */
	ia[3] = 2, ja[3] = 1, ar[3] =  .2; /* a[2,1] = .2 */
	ia[4] = 2, ja[4] = 2, ar[4] =  .4; /* a[2,2] =  .4 */
	ia[5] = 3, ja[5] = 1, ar[5] =  .35; /* a[3,1] =  .35 */
	ia[6] = 3, ja[6] = 2, ar[6] =  .2; /* a[3,2] =  .2 */
	glp_load_matrix(lp, 6, ia, ja, ar);

	glp_iocp parm;
  glp_init_iocp(&parm);
	parm.presolve = GLP_ON;
	glp_intopt(lp, &parm);

  z = glp_mip_obj_val(lp);
  x1 = glp_mip_col_val(lp, 1);
  x2 = glp_mip_col_val(lp, 2);
	printf("\nz = %g; x1 = %g; x2 = %g;\n", z, x1, x2);
	glp_delete_prob(lp);
	return 0;
}
/* eof */
