/*
 * xij: how many tons ships from company i to furniture facility j
  minimize
    z = 1*x11 + 3*x12 + 5*x13 +
        3.5*x21 + 4*x22 + 4.8*x23 +
        3.5*x31 + 3.6*x32 + 32*x33
  subject to
    f1 = x11 + x21 + x31
    f2 = x12 + x22 + x32
    f3 = x13 + x23 + x33
    c3 = x31 + x32 + x33
  and bounds of variables
    f1 >= 500
    f2 >= 700
    f3 >= 600
    c1 <= 500
    x21 <= 200
    x22 <= 200
    x23 <= 200
    x31 <= 200
    x32 <= 200
    x33 <= 200
*/

#include <cstdio>
#include <string>
#include <vector>
#include <stdlib.h>
#include <glpk.h>
using namespace std;

struct problem {
    glp_prob* lp;
    problem(const string& name) {
      lp = glp_create_prob();
      glp_set_prob_name(lp, name.c_str());
    }
    problem& minimize() { glp_set_obj_dir(lp, GLP_MIN); return *this; }
    problem& maximize() { glp_set_obj_dir(lp, GLP_MAX); return *this; }

    struct entry {
      string name;
      double val;
      double upval;
      int direction;
      double coef;
      bool isinteger;
      entry(const string& name = "") : name(name), val(0), direction(0), isinteger(false) { }
      entry& value(double value) { val = value; return *this; }
      entry& coefitent(double coefitent) { coef = coefitent; return *this; }
      entry& low() { direction = GLP_LO; return *this; }
      entry& up() { direction = GLP_UP; return *this; }
      entry& up(double up_val) { upval = up_val; direction = GLP_DB; return *this; }
      entry& as_integer() { isinteger = true; return *this; }
    };

    entry& row(const string& name) {
      rows.push_back(entry(name));
      return rows.back();
    }
    entry& col(const string& name) {
      cols.push_back(entry(name));
      return cols.back();
    }

    problem& apply_entries() {
      glp_add_rows(lp, rows.size());
      for(size_t i = 0; i < rows.size(); ++i) {
        glp_set_row_name(lp, i+1, rows[i].name.c_str());
        glp_set_row_bnds(lp, i+1, rows[i].direction, rows[i].val, rows[i].val);
      }
      glp_add_cols(lp, cols.size());
      for(size_t i = 0; i < cols.size(); ++i) {
        glp_set_col_name(lp, i+1, cols[i].name.c_str());
        if(cols[i].direction != GLP_DB) {
          glp_set_col_bnds(lp, i+1, cols[i].direction, cols[i].val, cols[i].val);
        } else {
          glp_set_col_bnds(lp, i+1, cols[i].direction, cols[i].val, cols[i].upval);
        }
        glp_set_obj_coef(lp, i+1, cols[i].coef);
        if(cols[i].isinteger) { glp_set_col_kind(lp, i+1, GLP_IV); }
      }
      return *this;
    }

    problem& matrix(vector<vector<double> > mtx) {
      int inx = 0;
      for(size_t i = 0; i < mtx.size(); ++i) {
        for(size_t j = 0; j < mtx[i].size(); ++j) {
          ++inx;
          ia[inx] = i+1; ja[inx] = j+1; ar[inx] = mtx[i][j];
        }
      }
      glp_load_matrix(lp, inx, ia, ja, ar);
      return *this;
    }

    double solve() {
      glp_iocp parm;
      glp_init_iocp(&parm);
      parm.presolve = GLP_ON;
      glp_intopt(lp, &parm);
      return glp_mip_obj_val(lp);
    }

    double col_value(int inx) {
      return glp_mip_col_val(lp, inx);
    }

    vector<entry> rows;
    vector<entry> cols;
    int ia[1+1000], ja[1+1000];
    double ar[1+1000];

    problem(problem&& rhs) : lp(rhs.lp) { rhs.lp = nullptr; }
    // TODO: write copy-ctor
    ~problem() {
      if(lp) {
        glp_delete_prob(lp);
        lp = nullptr;
      }
    }
};

int main(void) {
    problem p = problem("homework 1 - question 3");
    p.minimize();
    p.row("f1").value(500.0).low();
    p.row("f2").value(700.0).low();
    p.row("f3").value(600.0).low();
    p.row("c1").value(500.0).up();

    p.col("x11").value(0.0).low().coefitent(1.0).as_integer();
    p.col("x12").value(0.0).low().coefitent(3.0).as_integer();
    p.col("x13").value(0.0).low().coefitent(5.0).as_integer();

    p.col("x21").value(0.0).up(200.0).coefitent(3.5).as_integer();
    p.col("x22").value(0.0).up(200.0).coefitent(4.0).as_integer();
    p.col("x23").value(0.0).up(200.0).coefitent(4.8).as_integer();

    p.col("x31").value(0.0).up(200.0).coefitent(3.5).as_integer();
    p.col("x32").value(0.0).up(200.0).coefitent(3.6).as_integer();
    p.col("x33").value(0.0).up(200.0).coefitent(3.2).as_integer();

    p.apply_entries();


/*
    z = 1*x11 + 3*x12 + 5*x13 +
        3.5*x21 + 4*x22 + 4.8*x23 +
        3.5*x31 + 3.6*x32 + 32*x33
  subject to
    f1 = x11 + x21 + x31
    f2 = x12 + x22 + x32
    f3 = x13 + x23 + x33
    c3 = x31 + x32 + x33
*/
    vector<vector<double> > mtx = {
      {1, 0, 0, 1, 0, 0, 1, 0, 0},
      {0, 1, 0, 0, 1, 0, 0, 1, 0},
      {0, 0, 1, 0, 0, 1, 0, 0, 1},
      {0, 0, 0, 0, 0, 0, 1, 1, 1}
    };
    p.matrix(mtx);
    double z = p.solve();
    double x11 = p.col_value(1); double x12 = p.col_value(2); double x13 = p.col_value(3);
    double x21 = p.col_value(4); double x22 = p.col_value(5); double x23 = p.col_value(6);
    double x31 = p.col_value(7); double x32 = p.col_value(8); double x33 = p.col_value(9);
    printf("\nz = %g;\n"
    " x11 = %g; x12 = %g; x13 = %g;\n"
    " x21 = %g; x22 = %g; x23 = %g;\n"
    " x31 = %g; x32 = %g; x33 = %g;\n",
    z, x11, x12, x13, x21, x22, x23, x31, x32, x33);
    return 0;
}
