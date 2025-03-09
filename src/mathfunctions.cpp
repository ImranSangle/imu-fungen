

double rangemap(double v,double a,double b,double c,double d){

   double t = (v-a)/(b-a);

   return (c*(1.0-t))+(d*t);

}
