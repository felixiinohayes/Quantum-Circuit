#ifndef COMPLEX_H
#define COMPLEX_H

class complex
{
    private:
        double re,im;
    public:
        // Constructors & destructor
        complex() : re{},im{}{}
        complex(double r, double i) : re{r},im{i}{}
        ~complex(){}

        // Make functions to overload operators (<< and >>) friends
        friend std::ostream& operator<<(std::ostream &o, const complex &c);
        friend std::istream& operator>>(std::istream &i, complex &c);

        // Return real component
        double get_real()
        {
            return re;
        }
        // Return imaginary component
        double get_imaginary()
        {
            return im;
        }
        // Return modulus
        double get_modulus()
        {
            return sqrt((re * re) + (im * im));
        }
        // Return argument
        double get_argument()
        {
            return atan2(im, re);
        }
        // Return complex conjugate
        complex get_conjugate()
        {
            return complex(re, -im);
        }

        // Overload + operator for addition 
        complex operator+(const complex& c) const
        {
            return complex(re + c.re, im + c.im);
        }
        // Overload - operator for subtraction
        complex operator-(const complex& c) const
        {
            return complex(re - c.re, im - c.im);
        }
        // Overload * operator for multiplication, z1*z2
        complex operator*(const complex& c) const
        {
            double new_re{(re * c.re) - (im * c.im)};
            double new_im{(re * c.im) + (im * c.re)};
            return complex(new_re, new_im);
        }
        // Overload / operator for division, z1/z2
        complex operator/(const complex& c) const
        {
            double new_re{((im*c.re) - (re*c.im)) / ((c.re*c.re) + (c.im*c.im))};
            double new_im{((re*c.re) + (im*c.im)) / ((c.re*c.re) + (c.im*c.im))};
            return complex(new_re, new_im);
        }
        
};
// Overload << and >> operators for complex number insertion/extraction from iostreams
std::ostream &operator<<(std::ostream &o, const complex &c)
{
    if ((c.re == 0) && (c.im == 0)) {
        o << "0";
    } else if ((c.re != 0) && (c.im == 0)) {
        o << std::setprecision(3) << c.re;
    } else if ((c.re != 0) && (c.im > 1)) {
        o << std::setprecision(3) << c.re << "+" << c.im << "i" ;
    } else if ((c.re != 0) && (c.im < -1)) {
        o << std::setprecision(3) << c.re << c.im << "i";
    } else if (c.re == 0 && c.im != -1 && c.im != 1) {
        o << std::setprecision(3) << c.im << "i";
    } else if (c.im == 1) {
        o << "i";
    } else if (c.im == -1) {
        o << "-i";
    } else if (c.re == 1 || c.re == -1){
        o << c.re;
    }
    return o;
}
std::istream &operator>>(std::istream &i, complex &c)
{
    i >> c.re >> c.im;
    return i;
}
#endif