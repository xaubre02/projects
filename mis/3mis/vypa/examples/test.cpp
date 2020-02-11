class Shape : Object {
    int id;

    void init(int num){
        this.id = num;
    }
    int get_id(){
        return this.id;
    }
    void my_print(){
        print(this.id);
    }
}


void main(void) {
    Shape sh1;
    Shape sh2;
    Shape sh3;
    Shape sh1_c;

    sh1 = new Shape;
    sh2 = new Shape;
    sh1.init(5);
    sh2.init(2);
    sh3 = new Shape;
    sh1.my_print();
    print("\n");
    sh2.my_print();
    sh3.init(1);
    print("\n");
    sh3.my_print();
    print("\n");
    print("\n");
    int a;
    a = sh1.get_id();
    print("here: ");
    print(a);

    print("\n");
    print("\n");


    if(sh1 == sh2){
        print("true\n");
    } else {
        print("false\n");
    }

    sh1_c = sh1;
    if(sh1 == sh1_c){
        print("true\n");
    } else {
        print("false\n");
    }
} //