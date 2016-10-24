/*
 * xij: how many of product i is run on machine j
  minimize
        z = 4*x11*.3  + 4*x12*.25  + 5*x13*.2 + 7*x14*.2  +
        6*x21*.2  + 7*x22*.3  + 5*x23*.2 + 6*x24*.25  +
        12*x31*.8 + 10*x32*.6 + 8*x33*.6 + 11*x34*.5
  subject to
    p1 = x11 + x12 + x13 + x14
    p2 = x21 + x22  + x23 + x24
    p3 = x31 + x32  + x33 + x34
    m1 = x11*.3 + x21*.2 + x31*.8
    m2 = x12*.25 + x22*.3 + x32*.6
    m3 = x13*.2 + x23*.2 + x33*.6
    m4 = x14*.2 + x24*.25 + x34*.5
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
    p.row("p1").value(3000.0).low();
    p.row("p2").value(6000.0).low();
    p.row("p3").value(4000.0).low();
    p.row("m1").value(1500.0).up();
    p.row("m2").value(1200.0).up();
    p.row("m3").value(1500.0).up();
    p.row("m4").value(2000.0).up();

    p.col("x11").value(0.0).low().coefitent(4.0*.3).as_integer();
    p.col("x12").value(0.0).low().coefitent(4.0*.25).as_integer();
    p.col("x13").value(0.0).low().coefitent(5.0*.2).as_integer();
    p.col("x14").value(0.0).low().coefitent(7.0*.2).as_integer();

    p.col("x21").value(0.0).low().coefitent(6.0*.2).as_integer();
    p.col("x22").value(0.0).low().coefitent(7.0*.3).as_integer();
    p.col("x23").value(0.0).low().coefitent(5.0*.2).as_integer();
    p.col("x24").value(0.0).low().coefitent(6.0*.25).as_integer();

    p.col("x31").value(0.0).low().coefitent(12.0*.8).as_integer();
    p.col("x32").value(0.0).low().coefitent(10.0*.6).as_integer();
    p.col("x33").value(0.0).low().coefitent(8.0*.6).as_integer();
    p.col("x34").value(0.0).low().coefitent(11.0*.5).as_integer();

    p.apply_entries();

    vector<vector<double> > mtx = {
      {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1},
      {.3, 0, 0, 0, .2, 0, 0, 0, .8, 0, 0, 0}, // m1
      {0, .25, 0, 0, 0, .3, 0, 0, 0, .6, 0, 0}, // m2
      {0, 0, .2, 0, 0, 0, .2, 0, 0, 0, .6, 0}, // m3
      {0, 0, 0, .2, 0, 0, 0, .25, 0, 0, 0, .5}, // m4
    };
    p.matrix(mtx);
    double z = p.solve();
    double x11 = p.col_value(1); double x12 = p.col_value(2);
    double x13 = p.col_value(3); double x14 = p.col_value(4);
    double x21 = p.col_value(5); double x22 = p.col_value(6);
    double x23 = p.col_value(7); double x24 = p.col_value(8);
    double x31 = p.col_value(9); double x32 = p.col_value(10);
    double x33 = p.col_value(11); double x34 = p.col_value(12);
    printf("\nz = %g;\n"
    " x11 = %g; x12 = %g; x13 = %g; x14 = %g;\n"
    " x21 = %g; x22 = %g; x23 = %g; x24 = %g;\n"
    " x31 = %g; x32 = %g; x33 = %g; x34 = %g;\n",
    z, x11, x12, x13, x14, x21, x22, x23, x24, x31, x32, x33, x34);
    return 0;
}
