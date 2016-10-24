/*
 * xij: how many hours product i is run on machine j
  minimize
        z = 4*x11  + 4*x12  + 5*x13 + 7*x14  +
        6*x21  + 7*x22  + 5*x23 + 6*x24  +
        12*x31 + 10*x32 + 8*x33 + 11*x34
  subject to
    p1 = .3*x11 + .25*x12 + .2*x13 + .2*x14
    p2 = .2*x21 + .3*x22  + .2*x23 + .25*x24
    p3 = .8*x31 + .6*x32  + .6*x33 + .5*x34
    m1 = x11 + x21 + x31
    m2 = x12 + x22 + x32
    m3 = x13 + x23 + x33
    m4 = x14 + x24 + x34
  and bounds of variables
    p1 >= 3000
    p2 >= 6000
    p3 >= 4000
    m1 <= 1500
    m2 <= 1200
    m3 <= 1500
    m4 <= 2000
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
      int direction;
      double coef;
      bool isinteger;
      entry(const string& name = "") : name(name), val(0), direction(0), isinteger(false) { }
      entry& value(double value) { val = value; return *this; }
      entry& coefitent(double coefitent) { coef = coefitent; return *this; }
      entry& low() { direction = GLP_LO; return *this; }
      entry& up() { direction = GLP_UP; return *this; }
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
        glp_set_col_bnds(lp, i+1, cols[i].direction, cols[i].val, cols[i].val);
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
    problem p = problem("homework 1 - question 2");
    p.minimize();
    p.row("p").value(1000*1000.0).low();
    p.row("q").value(500*1000.0).low();
    p.row("r").value(300*1000.0).low();
    p.col("x1").value(0.0).low().coefitent(20.0).as_integer();
    p.col("x2").value(0.0).low().coefitent(15.0).as_integer();
    p.apply_entries();

    vector<vector<double> > mtx = {
      {.4, .32},
      {.2, .4},
      {.35, .2}
    };
    p.matrix(mtx);
    double z = p.solve();
    double x1 = p.col_value(1);
    double x2 = p.col_value(2);
    printf("\nz = %g; x1 = %g; x2 = %g;\n", z, x1, x2);
    return 0;
}
/* eof */
