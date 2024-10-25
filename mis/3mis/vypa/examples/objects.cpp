/* Program 4: Program with Classes and Objects */
class Shape : Object {
    int id;
    void Shape(void) { print("constructor of Shape"); }
    string toString() { return "instance of Shape " + (string)(this.id); }
}

class Rectangle : Shape {
    int height, width;
    string toString(void) { return super.toString()
                                   + " - rectangle " + (string)(this.area()); }
    int area() { return this.height * this.width; }
}

void main(void) {
    Rectangle r; r = new Rectangle;
    r.id = 42; r.width = readInt(); r.height = readInt();
    Shape s; s = r;
    print(s.toString());
} // end of main