/* Amplitude-adjusted PWD - linear operator */
/*
  Copyright (C) 2021 University of Texas at Austin
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <rsf.h>

#include "aapwd.h"

int main(int argc, char* argv[])
{
    bool adj, drift, forx;
    int m1, m2, n, n1, n2;
    float *xn, *x1, *dx, *r;
    sf_file inp, out, dip, sig;

    sf_init(argc,argv);
    inp = sf_input("in");
    dip = sf_input("dip");
    sig = sf_input("sig");
    out = sf_output("out");

    if (SF_FLOAT != sf_gettype(inp)) sf_error("Need float input");
    if (!sf_histint(dip,"n1",&m1)) sf_error("No n1= in dip");
    if (!sf_histint(dip,"n2",&m2)) sf_error("No n2= in dip");

    n = m1*m2;

    if (!sf_getbool("adj",&adj)) adj=false;
    /* adjoint flag */

    if (!sf_getbool("drift",&drift)) drift=false;
    /* if shift filter */

    if (!sf_getbool("forx",&forx)) forx=false;
    /* for x only */

    n2 = forx? 1:3;
    
    if (adj) {
	sf_putint(out,"n3",n2);
    } else {
	sf_putint(out,"n3",2);
    }

    n2 = n2*n; 
    n1 = 2*n;

    x1 = sf_floatalloc(n);
    sf_floatread(x1,n,sig);

    xn = sf_floatalloc(3*n);
    sf_floatread(xn,n,dip);

    aapwd_init(m1,m2,1,drift,x1,xn,xn+n,xn+2*n);
    
    dx = sf_floatalloc(n2);
    r =  sf_floatalloc(n1);

    if (adj) {
	sf_floatread(r,n1,inp);
    } else {
	sf_floatread(dx,n2,inp);
    } 

    if (forx) {
	aapwdx_lop(adj,false,n2,n1,dx,r);
    } else {
	/* aapwd_lop(adj,false,n2,n1,dx,r); */
    }

    if (adj) {
	sf_floatwrite(dx,n2,out);
    } else {
	sf_floatwrite(r,n1,out);
    } 

    exit(0);
}
