/* Written by M A H Newton on 25.08.2019 */

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>


using namespace std;

/* A class to display 2d graphics in text mode */
/* The screen is a square (-dmin,-dmin) to (dmax,dmax) */
/* The typical screen size is (-20, -20) to (20,20) */
class screen
{
public:
    screen(); 												// default constructor
    screen(screen const & that);							// copy constructor
    screen(screen && that) noexcept;						// move constructor
    screen & operator = (screen const & that); 				// copy assignment
    screen & operator = (screen && that) noexcept;			// move assignment
    ~screen();												// destructor

    int dmin() const;										// getter for dmin
    int dmax() const;										// getter for dmax

    void display() const;									// display screen

    void clear();											// clear the screen
    void axes(char s);										// show both axes using symbol symb

    void point(int x, int y, char s);						// plot a point using coordinate (x,y) and symbol symb
    void ellipse(int x, int y, int a, int b, char s);		// plot an ellipse using center (x,y), radii a and b, and symbol symb
    void polygon(int x, int y, int n, int l, char s);		// plot a polygon using center (x,y), n sides, length l, and symbol symb

    void line(int x, int y, int xp, int yp, char s);		// plot a line from (x,y) to (xp, yp) using symbol symb

private:

    // low lovel function to plot a single point  since y : x ratio is not 1, x is multiplied if r is true
    void plot(double x, double y, char s);			// plot a point

    int const dim = 20;								// default dimension
    double const ratio = 2.23;						// aspect ratio
    double const pi = acos(-1);						// pi value

    int mWidth, mHeight;							// width and height
    char ** mBoard;									// 2d screen board
};

// default constructor
screen::screen() : mWidth(ratio * dim * 2 + 1), mHeight(dim * 2 + 1)
{
    mBoard = new char * [mHeight];
    for(int j = 0; j < mHeight; ++j)
        mBoard[j] = new char [mWidth]();
}

// destructor
screen::~screen()
{
    for(int j = 0; j < mHeight; ++j)
        delete [] mBoard[j];
    delete mBoard;
}

// copy constructor
screen::screen(screen const & that) : mWidth(that.mWidth), mHeight(that.mHeight)
{
    mBoard = new char * [mHeight];
    for(int j = 0; j < mHeight; ++j)
    {
        mBoard[j] = new char [mWidth];
        for (int i = 0; i < mWidth; ++i)
            mBoard[j][i] = that.mBoard[j][i];
    }
}

// move constructor
screen::screen(screen && that) noexcept :
        mWidth(move(that.mWidth)), mHeight(move(that.mHeight)), mBoard(move(that.mBoard))
{
    that.mBoard = nullptr;
    that.mWidth = 0;
    that.mHeight = 0;
}

// copy assignment
screen & screen::operator = (screen const & that)
{
    if (this == &that) return *this;

    if (mHeight != that.mHeight || mWidth != that.mWidth)
    {
        for(int j = 0; j < mHeight; ++j)
            delete [] mBoard[j];
        delete mBoard;
        mWidth = that.mWidth;
        mHeight = that.mHeight;
        mBoard = new char * [mHeight];
        for(int j = 0; j < mHeight; ++j)
            mBoard[j] = new char [mWidth]();
    }

    for(int j = 0; j < mHeight; ++j)
        for (int i = 0; i < mWidth; ++i)
            mBoard[j][i] = that.mBoard[j][i];

    return *this;
}

// move constructor
screen & screen::operator = (screen && that) noexcept
{
    mWidth = move(that.mWidth);
    mHeight = move(that.mHeight);
    mBoard = move(that.mBoard);

    that.mBoard = nullptr;
    that.mWidth = 0;
    that.mHeight = 0;

    return *this;
}

// plot a point
void screen::plot(double x, double y, char s)
{
    int yy = static_cast<int>(round(-y + mHeight / 2) );
    int xx = static_cast<int>(round(ratio * x + mWidth / 2 + ratio / 2 ));
    mBoard[yy][xx] = s;
}

// getter for dmax
int screen::dmax() const
{
    return dim;
}

// getter for dmin
int screen::dmin() const
{
    return - dim;
}

// display screen
void screen::display() const
{
    for(int j = 0; j < mHeight; ++j)
    {
        for(int i = 0; i < mWidth; ++i)
            if (mBoard[j][i])
                cout << mBoard[j][i];
            else
                cout << ' ';
        cout << endl;
    }
}

// clear the screen
void screen::clear()
{
    for(int j = 0; j < mHeight; ++j)
        for(int i = 0; i < mWidth; ++i)
            mBoard[j][i] = 0;
}

// show the axes using symbol symb
void screen::axes(char s)
{
    // plot x axis
    for(int x = dmin(); x <= dmax(); ++x)
        plot(x,0,s);
    // plot y axis
    for(int y = dmin(); y <= dmax(); ++y)
        plot(0,y,s);
}

// plot a point at (x,y) with symbol symb
void screen::point(int x, int y, char s)
{
    plot(static_cast<double>(x), static_cast<double>(y), s);
}

// plot an ellipse using center (xc, yc), radii rx and ry, and symbol symb
void screen::ellipse(int xc, int yc, int rx, int ry, char s)
{
    double dx, dy, d1, d2, x, y;
    x = 0;
    y = ry;

    // initial decision parameter of region 1
    d1 = (ry * ry) - (rx * rx * ry) + (0.25 * rx * rx);
    dx = 2 * ry * ry * x;
    dy = 2 * rx * rx * y;

    // for region 1
    while (dx < dy)
    {
        // print point based on 4-way symmetry
        plot(x + xc, y + yc, s);
        plot(-x + xc, y + yc, s);
        plot(x + xc, -y + yc, s);
        plot(-x + xc, -y + yc, s);

        // checking and updating value of
        // decision parameter based on algorithm
        if (d1 < 0)
        {
            x++;
            dx = dx + (2 * ry * ry);
            d1 = d1 + dx + (ry * ry);
        }
        else
        {
            x++;
            y--;
            dx = dx + (2 * ry * ry);
            dy = dy - (2 * rx * rx);
            d1 = d1 + dx - dy + (ry * ry);
        }
    }

    // decision parameter of region 2
    d2 = ((ry * ry) * ((x + 0.5) * (x + 0.5))) +
         ((rx * rx) * ((y - 1) * (y - 1))) -
         (rx * rx * ry * ry);

    // plotting point of region 2
    while (y >= 0)
    {
        // print point based on 4-way symmetry
        plot(x + xc, y + yc, s);
        plot(-x + xc, y + yc, s);
        plot(x + xc, -y + yc, s);
        plot(-x + xc, -y + yc, s);

        // checking and updating parameter
        // value based on algorithm
        if (d2 > 0)
        {
            y--;
            dy = dy - (2 * rx * rx);
            d2 = d2 + (rx * rx) - dy;
        }
        else
        {
            y--;
            x++;
            dx = dx + (2 * ry * ry);
            dy = dy - (2 * rx * rx);
            d2 = d2 + dx - dy + (rx * rx);
        }
    }
}

// plot a regular polygon with center (x,y), length l, n sides, using symbol symb
void screen::polygon(int x, int y, int n, int l, char s)
{
    double * xx = new double [n];
    double * yy = new double [n];
    for(int k = 0; k < n; ++k)
    {
        xx[k] = l * cos(2*pi*k/n);
        yy[k] = l * sin(2*pi*k/n);
    }
    for(int k = 0; k < n; ++k)
        line(x + xx[k], y + yy[k], x + xx[(k+1) % n], y + yy[(k+1) % n],s);
}

// plot a line from (x0,y0) to (x1, y1) using symbol symb
void screen::line(int x0, int y0, int x1, int y1, char s)
{
    if (x1 == x0)
    {
        if (y1 > y0)
            for(double y = y0; y <= y1; y++)
                plot(x0, y, s);
        else
            for(double y = y1; y <= y0; y++)
                plot(x0, y, s);
        return;
    }

    if (y1 == y0)
    {
        if (x1 > x0)
            for(double x = x0; x <= x1; x++)
                plot(x, y0, s);
        else
            for(double x = x1; x <= x0; x++)
                plot(x, y0, s);
        return;
    }

    double m = 1.0 * (y1 - y0) / (x1 - x0);
    double c = 1.0 * (y0 * x1 - y1 * x0) / (x1 - x0);

    if (abs(x1-x0) > abs(y1 - y0))
    {
        if (x1 > x0)
            for(double x = x0; x <= x1; x+=1)
                plot(x, m * x + c, s);
        else
            for(double x = x1; x <= x0; x+=1)
                plot(x, m * x + c, s);
    }
    else
    {
        if (y1 > y0)
            for(double y = y0; y <= y1; y+=1)
                plot((y - c)/m, y, s);
        else
            for(double y = y1; y <= y0; y+=1)
                plot((y - c)/m, y, s);
    }
}
//______SCREEN END__________






//_______POINTS CLASS___________
class point
{
public:
    //def constructor
    point(){
            x=0;
            y=0;
            symb='*';

    }
    //constructor
    point(int pointx, int pointy, char pointz){
        x=pointx;
        y=pointy;
        symb=pointz;
    }
    //copy constructor
    point(const point & that){
        x = that.x;
        y = that.y;
        symb = that.symb;
    }

    //move constructor
    point(point && that) noexcept: x(move(that.x)), y(move(that.y)), symb(move(that.symb)){
        that.x = 0;
        that.y = 0;
        that.symb = 0;
    }

    //move assignment
    point & operator = (const point & that){
        x = that.x;
        y = that.y;
        symb = that.symb;
        return *this;
    }

    int X();
    int Y();
    char SYM();

    void info();
    void draw();

private:
    int x;
    int y;
    char symb;
};

int point::X(){
    return x;
}

int point::Y(){
    return y;
}

char point::SYM(){
    return symb;
}

void point::info(){
    cout << "Point coords: " <<x<<" "<<y<<" "<<symb<< endl;
}

void point::draw(){
    screen scrn;
    scrn.point(x, y, symb);
    scrn.display();
}
//______END OF POINT CLASS________


class line
{
public:
    line(){
        x = 0;
        y = 0;
        a = 0;
        b = 0;
        sym = '#';
    }

    line(int coordx, int coordy, int coord_a, int coord_b, char coord_s){
        x = coordx;
        y = coordy;
        a = coord_a;
        b = coord_b;
        sym = coord_s;
    }

    line(line && that) noexcept : x(move(that.x)), y(move(that.y)), sym(move(that.sym)) {that.x = 0; that.y = 0; that.a = 0; that.b = 0; that.sym = 0;}

    line & operator = (const line& that) {
        x = that.x;
        y = that.y;
        a = that.a;
        b = that.b;
        sym = that.sym;

        return *this;
    }

    line(const line & that){
        x=that.x;
        y=that.y;
        a=that.a;
        b=that.b;
        sym=that.sym;
    }

    int X();
    int Y();
    int A();
    int B();
    char SYM();

    void info();
    void draw();

private:
    int x,y,a,b; char sym;
};

int line::X(){
    return x;
}

int line::Y(){
    return y;
}
int line::A(){
    return a;
}
int line::B(){
    return b;
}
char line::SYM(){
    return sym;
}

void line::info() {
    cout << "Line coords: "<<x<<" "<<y<<" "
         <<a<<" "<< b<<" "<<sym<< endl;
}

void line::draw(){
    screen sc;
    sc.line(x, y, a, b, sym);
    sc.display();
}
//________END OF LINES CLASS________



class ellipse{
public:
    ellipse(){
        x=0;
        y=0;
        a=0;
        sym='+';
    }

    ellipse(int coord_x, int coord_y, int coord_a, int coord_b, char coord_s){
        x=coord_x;
        y=coord_y;
        a=coord_a;
        b=coord_b;
        sym=coord_s;
    }

    ellipse(const ellipse & that){
        x=that.x; y = that.y;
        a = that.a; b = that.b;
        sym = that.sym;
    }

    ellipse & operator = (const ellipse& that){
        x = that.x;
        y = that.y;
        a = that.a;
        b = that.b;
        sym = that.sym;

        return *this;
    }

    ellipse(ellipse && that) noexcept : x(move(that.x)), y(move(that.y)), sym(move(that.sym)) {
        that.x = 0;
        that.y = 0;
        that.a = 0;
        that.b = 0;
        that.sym = 0;

    }

    int X();
    int Y();
    int A();
    int B();
    char S();

    void info();
    void draw();

private:
    int x,y,a,b;
    char sym;
};

int ellipse::X(){
    return x;
}

int ellipse::Y(){
    return y;
}

int ellipse::A(){
    return a;
}

int ellipse::B(){
    return b;
}

char ellipse::S(){
    return sym;
}

void ellipse::info(){
    cout << "ellipse " <<x<<" "<<y<<" "<<a<<" "<<b<<" "<<sym<< endl;
}

void ellipse::draw(){
    screen sc;
    sc.ellipse(x, y, a, b, sym);
    sc.display();
}
//_________END OF ELLIPSES CLASS_______


class polygons
{
public:
    polygons(){
        x = 0;
        y = 0;
        a = 0;
        b = 0;
        s = '$';
    }

    polygons(int pointx, int pointy, int pointa, int pointb, char points){
        x = pointx;
        y = pointy;
        a = pointa;
        b = pointb;
        s = points;
    }

    polygons(const polygons & that){
        x = that.x;
        y = that.y;
        a = that.a;
        b = that.b;
        s = that.s;
    }

    polygons & operator = (const polygons& that) {
        x = that.x;
        y = that.y;
        a = that.a;
        b = that.b;
        s = that.s;

        return *this;
    }

    polygons(polygons && that) noexcept : x(move(that.x)), y(move(that.y)), s(move(that.s)){
        that.x = 0;
        that.y = 0;
        that.a = 0;
        that.b = 0;
        that.s = 0;
    }

    int X();
    int Y();
    int A();
    int B();
    char S();

    void info(){
        cout << "polygon " <<x<<" "<<y<<" "
             <<a<<" "<<b<<" "<<s<< endl;
    }

    void draw(){
        screen sc;
        sc.polygon(x, y, a, b, s);
        sc.display();
    }

private:
    int x,y,a,b; char s;
};

int polygons::X() {
    return x;
}

int polygons::Y(){
    return y;
}

int polygons::A(){
    return a;
}

int polygons::B(){
    return b;
}

char polygons::S(){
    return s;
}
//_______END OF POLYGONS CLASS_____


template <class T>
class bunch
{
public:
    bunch();

    void ADD(T n);
    int REMOVE();
    int EMPTY();
    int FULL();
    int SIZE();

    T & operator[](int pos){
        if(pos >= first+1){cout << "Wrong position! " << endl;}
        else{return number[pos];}
    }

private:
    T number[32];
    int first;
};

template <class T>
bunch<T>::bunch(){
    first = -1;
}

template <class T>
void bunch<T>::ADD(T n){
    ++first;
    number[first] = n;
}

template <class T>
int bunch<T>::REMOVE(){
    int tempStorage;

    if(EMPTY()){ return 0;}
    tempStorage = number[first];
    number[first] = 0;
    --first;

    return tempStorage;
}

template <class T>
int bunch<T>::EMPTY() {
    if(first==-1){return 1;}
    else{return 0;}
}

template <class T>
int bunch<T>::FULL(){
    if(first==(32 - 1)){return 1;}
    else{return 0;}
}

template <class T>
int bunch<T>::SIZE(){
    return first+1;
}
//__END OF BUNCH TEMPLATE___


//_______MAIN FUN_______
int main() {
    bunch<point> pbunch;
    bunch<polygons> polbunch;
    bunch<line> lbunch;
    bunch<ellipse> ebunch;

    bool quit = false;

    int option = 0;

    while(!quit){
        cout << "1. Draw a Point" << endl;
        cout << "2. Draw an Ellipse" << endl;
        cout << "3. Draw a Polygon" << endl;
        cout << "4. Draw a Line" << endl;
        cout << "5. Print all the items in a list" << endl;
        cout << "6. Draw a Cross <3" << endl;

        cin >> option;

        if(option == 1){
            cout << "For Point enter: X Y coord"<< endl;
            int x = 0, y =0;
            cin >> x >> y;
            point point(x , y, '*');
            pbunch.ADD(point); // this part no work
            point.info();
            point.draw();
        }

        if(option == 2){
            cout << "For Ellipse enter: X Y A B coord"<< endl;

            int x =0, y=0, a=0, b=0;
            cin >> x >> y >> a >> b ;

            ellipse ellipse(x,y,a,b,'+');

            ellipse.info();
            ellipse.draw();
        }

        if(option == 3){
            cout << "For Polygon enter: X Y A B coord"<< endl;

            int x =0, y=0, a=0, b=0;
            cin >> x >> y >> a >> b ;

            polygons polygon(x, y, a, b, '$');

            polygon.info();
            polygon.draw();
        }

        if(option == 4){
            cout << "For Line enter X Y A B coord"<< endl;

            int x =0, y=0, a=0, b=0;
            cin >> x >> y >> a >> b ;

            line line(x,y,a,b,'#');

            line.info();
            line.draw();
        }

        if(option == 5){
            if(pbunch.SIZE() != 0){
                for(int a=0; a < pbunch.SIZE(); a++){
                    point p;
                    p = pbunch[a];
                    p.info();
                }
            }
            if(ebunch.SIZE() != 0){
                for(int a=0; a < ebunch.SIZE(); a++){
                    ellipse e;
                    e = ebunch[a];
                    e.info();
                }
            }
            if(polbunch.SIZE() != 0){
                for (int a = 0; a < polbunch.SIZE(); a++) {
                    polygons p;
                    p = polbunch[a];
                    p.info();
                }
            }
            if (lbunch.SIZE() != 0) {
                for (int a = 0; a < lbunch.SIZE(); a++) {
                    line l;
                    l = lbunch[a];
                    l.info();
                }
            }
            if(lbunch.SIZE() == 0 && ebunch.SIZE() == 0 && polbunch.SIZE() == 0 && pbunch.SIZE() == 0){
                cout << "No items in Array. " << endl;
            }
        }

        if(option == 6){
            screen s;

            line l(0, -3, 6, -12, '+');
            lbunch.ADD(l);

            line l1(0,-3,0,-6,'+');
            lbunch.ADD(l1);

            for(int a =0; a < pbunch.SIZE(); a++){
                point p;
                p = pbunch[a];
                s.point(p.X(), p.Y(), p.SYM());
            }

            for(int a =0; a < ebunch.SIZE(); a++){
                ellipse ellip;
                ellip = ebunch[a];
                s.ellipse(ellip.X(),ellip.Y(),ellip.A(),ellip.B(),ellip.S());
            }

            for(int a =0; a < polbunch.SIZE(); a++){
                polygons polly;
                polly = polbunch[a];
                s.polygon(polly.X(),polly.Y(),polly.A(),polly.B(),polly.S());
            }

            for(int a =0; a < lbunch.SIZE(); a++){
                line line;
                line = lbunch[a];
                s.line(line.X(),line.Y(),line.A(),line.B(), line.SYM());
            }
            s.display();
        }


        if(option==10){quit = true;}
    };

    return EXIT_SUCCESS;
}