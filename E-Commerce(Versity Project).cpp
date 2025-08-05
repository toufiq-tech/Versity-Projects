#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
using namespace std;

// ----------------- ENUM -----------------
enum OrderStatus { PENDING, DELIVERED };

// ----------------- PRODUCT CLASS -----------------
class Product {
    int id;
    string name;
    double price;
    int quantity;
public:
    Product(int i, string n, double p, int q) {
        id = i;
        name = n;
        price = p;
        quantity = q;
    }
    int getId() { return id; }
    string getName() { return name; }
    double getPrice() { return price; }
    int getQuantity() { return quantity; }
    void setQuantity(int q) {
        quantity = q;
    }
    void show() {
        cout << "ID: " << id << " | Name: " << name << " | Price: " << price << "Tk | Qty: " << quantity << endl;
    }

    string toString() {
        return to_string(id) + " " + name + " " + to_string(price) + " " + to_string(quantity);
    }
    static Product fromLine(string line) {
        stringstream ss(line);
        int i, q;
        string n;
        double p;
        ss >> i >> n >> p >> q;
        return Product(i, n, p, q);
    }
};
// ----------------- ORDER CLASS -----------------
class Order {
    static int orderCounter;
    int id;
    OrderStatus status;
    vector<int> productIds;
public:
    Order(vector<int> ids) {
        id = ++orderCounter;
        status = PENDING;
        productIds = ids;
    }
    Order(int i, OrderStatus s, vector<int> ids) {
        id = i;
        status = s;
        productIds = ids;
        if (i > orderCounter) orderCounter = i;
    }
    int getId() { return id; }
    void setStatus(OrderStatus s) { status = s; }
    void show() {
        cout << "Order ID: " << id << " | Status: ";
        if (status == PENDING) cout << "Pending";
        else cout << "Delivered";
        cout << " | Products: ";
        for (int pid : productIds) cout << pid << " ";
        cout << endl;
    }
    string toString() {
        string line = to_string(id) + "|";
        line += (status == PENDING ? "PENDING|" : "DELIVERED|");

        for (int i = 0; i < productIds.size(); i++) {
            line += to_string(productIds[i]);
            if (i != productIds.size() - 1) line += ",";
        }
        return line;
    }
    static Order fromLine(string line) {
        stringstream ss(line);
        string idStr, statusStr, idsStr;
        getline(ss, idStr, '|');
        getline(ss, statusStr, '|');
        getline(ss, idsStr);

        int id = stoi(idStr);
        OrderStatus s = (statusStr == "PENDING") ? PENDING : DELIVERED;
        vector<int> ids;
        stringstream idStream(idsStr);
        string token;
        while (getline(idStream, token, ',')) {
            ids.push_back(stoi(token));
        }
        return Order(id, s, ids);
    }
};
int Order::orderCounter = 0;

// ----------------- USER CLASSES -----------------
class User {
protected:
    string username;
    string password;
public:
    User(string uname, string pwd) {
        username = uname;
        password = pwd;
    }
    virtual ~User() {}
    string getUsername() { return username; }
    virtual void showMenu() = 0;
};
class Admin : public User {
public:
    Admin(string uname, string pwd) : User(uname, pwd) {}
    void showMenu() override {
        cout << "\nAdmin Menu:\n";
        cout << "1. Add Product\n2. Edit Product\n3. View Orders\n4. Mark Order Delivered\n5. View Products\n0. Logout\nChoice: ";
    }
};
class Customer : public User {
public:
    Customer(string uname, string pwd) : User(uname, pwd) {}

    void showMenu() override {
        cout << "\nUser Menu:\n";
        cout << "1. View Products\n2. Search Product\n3. Add to Cart\n4. View Cart\n5. Place Order\n6. View Orders\n0. Logout\nChoice: ";
    }
};
// ----------------- FILE FUNCTIONS -----------------
vector<Product> loadProducts(string filename) {
    vector<Product> products;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        if (!line.empty()) {
            products.push_back(Product::fromLine(line));
        }
    }
    return products;
}
void saveProducts(vector<Product> products, string filename) {
    ofstream file(filename);
    for (Product p : products) {
        file << p.toString() << endl;
    }
}
vector<Order> loadOrders(string filename) {
    vector<Order> orders;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        if (!line.empty()) {
            orders.push_back(Order::fromLine(line));
        }
    }
    return orders;
}
void saveOrders(vector<Order> orders, string filename) {
    ofstream file(filename);
    for (Order o : orders) {
        file << o.toString() << endl;
    }
}
bool checkAdmin(string user, string pass) {
    ifstream file("admin.txt");
    string u, p;
    while (file >> u >> p) {
        if (u == user && p == pass) return true;
    }
    return false;
}
bool checkUser(string user, string pass) {
    ifstream file("users.txt");
    string u, p;
    while (file >> u >> p) {
        if (u == user && p == pass) return true;
    }
    return false;
}
bool userExists(string user) {
    ifstream file("users.txt");
    string u, p;
    while (file >> u >> p) {
        if (u == user) return true;
    }
    return false;
}
void registerUser(string user, string pass) {
    ofstream file("users.txt", ios::app);
    file << user << " " << pass << endl;
}
// ----------------- MAIN FUNCTION -----------------
int main() {
    vector<Product> products = loadProducts("products.txt");
    vector<Order> orders = loadOrders("orders.txt");
    int mainChoice;
    string username, password;
    do {
        cout << "\n1. Register\n2. Login\n0. Exit\nChoice: ";
        cin >> mainChoice;
        if (mainChoice == 1) {
            cout << "Username: ";
            cin >> username;
            if (userExists(username)) {
                cout << "Username already exists.\n";
            }
            else {
                cout << "Password: ";
                cin >> password;
                registerUser(username, password);
                cout << "User registered!\n";
            }
        }
        else if (mainChoice == 2) {
            cout << "Username: ";
            cin >> username;
            cout << "Password: ";
            cin >> password;
            if (checkAdmin(username, password)) {
                Admin admin(username, password);
                int choice;
                do {
                    admin.showMenu();
                    cin >> choice;
                    if (choice == 1) {
                        int id, qty;
                        string name;
                        double price;
                        cout << "ID: ";
                        cin >> id;
                        bool exists = false;
                        for (Product p : products) {
                            if (p.getId() == id) exists = true;
                        }
                        if (exists) {
                            cout << "Product already exists.\n";
                        }
                        else {
                            cout << "Name: ";
                            cin >> name;
                            cout << "Price: ";
                            cin >> price;
                            cout << "Quantity: ";
                            cin >> qty;
                            products.push_back(Product(id, name, price, qty));
                            saveProducts(products, "products.txt");
                        }
                    }
                    else if (choice == 2) {
                        int id, newQty;
                        cout << "Product ID: ";
                        cin >> id;
                        cout << "New Quantity: ";
                        cin >> newQty;
                        for (Product& p : products) {
                            if (p.getId() == id) {
                                p.setQuantity(newQty);
                            }
                        }
                        saveProducts(products, "products.txt");
                    }
                    else if (choice == 3) {
                        for (Order o : orders) o.show();
                    }
                    else if (choice == 4) {
                        int oid;
                        cout << "Order ID: ";
                        cin >> oid;
                        for (Order& o : orders) {
                            if (o.getId() == oid) o.setStatus(DELIVERED);
                        }
                        saveOrders(orders, "orders.txt");
                    }
                    else if (choice == 5) {
                        for (Product p : products) p.show();
                    }

                } while (choice != 0);
            }
            else if (checkUser(username, password)) {
                Customer customer(username, password);
                vector<pair<int, int>> cart;
                int choice;
                do {
                    customer.showMenu();
                    cin >> choice;

                    if (choice == 1) {
                        for (Product p : products) p.show();
                    }
                    else if (choice == 2) {
                        string key;
                        cout << "Search keyword: ";
                        cin >> key;
                        for (Product p : products) {
                            if (p.getName().find(key) != string::npos) {
                                p.show();
                            }
                        }
                    }
                    else if (choice == 3) {
                        int id, qty;
                        cout << "Product ID: ";
                        cin >> id;
                        cout << "Quantity: ";
                        cin >> qty;
                        cart.push_back(make_pair(id, qty));
                    }
                    else if (choice == 4) {
                        double total = 0;
                        cout << "Your Cart:\n";
                        for (auto item : cart) {
                            for (Product p : products) {
                                if (p.getId() == item.first) {
                                    double subtotal = p.getPrice() * item.second;
                                    total += subtotal;
                                    cout << p.getName() << " x" << item.second << " = " << subtotal << "Tk"<< endl;
                                }
                            }
                        }
                        cout << "Total: " << total << "Tk"<<endl;
                    }
                    else if (choice == 5) {
                        bool canOrder = true;
                        for (auto item : cart) {
                            for (Product p : products) {
                                if (p.getId() == item.first && p.getQuantity() < item.second) {
                                    cout << "Not enough stock for " << p.getName() << endl;
                                    canOrder = false;
                                }
                            }
                        }
                        if (canOrder) {
                            for (auto item : cart) {
                                for (Product& p : products) {
                                    if (p.getId() == item.first) {
                                        p.setQuantity(p.getQuantity() - item.second);
                                    }
                                }
                            }
                            vector<int> ids;
                            for (auto item : cart) {
                                for (int i = 0; i < item.second; i++) {
                                    ids.push_back(item.first);
                                }
                            }
                            orders.push_back(Order(ids));
                            saveOrders(orders, "orders.txt");
                            saveProducts(products, "products.txt");
                            cart.clear();
                            cout << "Order placed!\n";
                        }
                        else {
                            cout << "Could not place order.\n";
                        }
                    }
                    else if (choice == 6) {
                        for (Order o : orders) o.show();
                    }

                } while (choice != 0);
            }
            else {
                cout << "Invalid login!\n";
            }
        }
    } while (mainChoice != 0);
    return 0;
}
