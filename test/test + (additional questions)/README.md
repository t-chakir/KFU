# Список оставшихся вопросов теста Чабанова

ВАЖНО!
Владимир Чабанов может заменить какое-то задание! Уже есть пару задач, которые он заменил. Поэтому не удивляемся если задач больше чем 107. Те, о которых мне будут сообщать я буду подписывать как «НОВАЯ!».

---

Дана лямбда-функция на языке С++. Какой её тип возвращаемого значения?
```cpp
[](int a, int b){return a > b;};
```

Ответ:
- [ ] double
- [x] bool
- [ ] Никакого, т.к. тип не задан явно
- [ ] int

---

Язык С++. Что из нижеперечисленного приведёт к объявлению структуры?

Ответ:
<ul>
<li>- [x] a. <li>
        
```cpp
struct {
    int a;
} s;
```   
</li></li>

<li>- [ ] b. <li>
        
```cpp
struct a_struct {
    int a;
}
```   
</li></li>

<li>- [ ] c. <li>
        
```cpp
struct a_struct int a;
```   
</li></li>

<li>- [x] d. <li>
        
```cpp
struct a_struct{
    int a;
};
```   
</li></li>

<li>- [ ] e. <li>
        
```cpp
struct {
    int a;
}
```   
</li></li>

---

Дан фрагмент кода на языке С++. Что выведется на экран в результате его работы?

```cpp
enum Color {
    YELLOW,
    BLACK,
    PINK,
    GREEN
};

Color color = Color::BLACK;
std::cout << color;
```

Ответ:
- [ ] a. 0
- [ ] b. Ошибка. Для Color не определён оператор <<
- [x] c. 1
- [ ] d. Какое-то "мусорное" значение, т.к. для BLACK НЕ указано значение явно
- [ ] e. Строка BLACK

---

Дан фрагмент кода на С++. Что будет содержать переменная `container` после его выполнения?

```cpp
std::map<int, int> container{{1, 2}, {3, 2}};
container[-1] = 5;
```

Ответ:
- [ ] a. Ошибка доступа - указанного элемента не существует
- [x] b. Пары: -1:5, 1:2, 3:2
- [ ] c. Пары: 1:2, 3:2, -1:5
- [ ] d. Ошибка компиляции - недопустимый тип
- [ ] e. Пары: 1:2, 3:2, 5:-1

---

Язык Go. Что из нижеперечисленного приведёт к объявлению структуры?

Ответ:
- [x] a. 
```go
var Address struct {
    Name    string
    City    string
    Pincode int
}
```

- [x] b. 
```go
pizza := struct {
    address string 
    name string
    cost int
}{
    address: "address",
    name:    "Pizza",
    cost:    100,
}
```

- [ ] c. 
```go
john := Person {
    Name: "John Doe",
    Age:  30,
}
```

- [x] d. 
```go
type Mail = struct {
    Address string
    Message string
    Code int
}
```

- [x] e. 
```go
type Employee struct {
    name string
    age int
}
```

---

Дан фрагмент кода на С++. Что будет содержать переменная `container` после его выполнения?

```cpp
std::set<int> container{1, 2, 3, 4};
container.insert(container.begin(), 4);
```

Ответ:
- [ ] a. 1, 2, 3, 4, 4
- [ ] b. 4, 1, 2, 3, 4
- [ ] c. 1, 2, 3, 4 но в порядке зависящем от хэш-функции
- [x] d. 1, 2, 3, 4

---

Дан фрагмент кода на Go. Что будет содержать переменная `container` после его выполнения?

```go
var container map[int]int
container[-1] = 1
```

Ответ:
- [ ] a. Ошибка компиляции - недопустимый тип
- [x] b. Ошибка во время исполнения - под словарь не выделена память
- [ ] c. Ключ: -1 со значением: 0
- [ ] d. Ошибка во время исполнения - попытка доступа к несуществующему ключу
- [ ] e. Ключ: -1 со значением: 1

---

Дан фрагмент кода на С++. Что будет содержать переменная `container` после его выполнения?

```cpp
std::vector<int> container{1, 2, 3, 4};
container.insert(container.begin(), 4);
```

Ответ:
- [x] a. 4, 1, 2, 3, 4
- [ ] b. 1, 2, 3, 4 но в порядке зависящем от хэш-функции
- [ ] c. 1, 2, 3, 4
- [ ] d. 1, 2, 3, 4, 4

---

Дан фрагмент кода на С++ и класс `MyClass` объявленный как:

```cpp
class MyClass{};
```

Выберите все верные варианты, которые являются допустимыми объявлениями.

Ответ:
- [x] a. 
```cpp
double operator+ (double a, double b);
```

- [ ] b. 
```cpp
MyClass operator+ (double a, double b);
```

- [x] c. 
```cpp
double operator+ (MyClass a, double b);
```

- [x] d. 
```cpp
double operator+ (MyClass a, MyClass b);
```

- [x] e. 
```cpp
double operator+ (double a, MyClass b);
```

---

Язык Go. Какие поля могут быть в структурном типе?

Ответ:
- [ ] a. Константные поля;
- [x] b. Анонимные поля;
- [ ] c. Поля-ссылки;
- [x] d. Не константные поля;
- [ ] e. Статические поля;

---

Дан код на Go. Что будет выведено на экран в результате его выполнения:

```go
package main
import "fmt"

func change(abc []int) {
    for i := range abc {
        abc[i] = 4
    }
    fmt.Println(abc)
}

func main() {
    abc := []int{1, 2, 3}
    change(abc)
    fmt.Println(abc)
}
```

Ответ:
- [ ] a. 
```
[4 4 4]
[1 2 3]
```

- [x] b. 
```
[4 4 4]
[4 4 4]
```

- [ ] c. 
```
[1 2 4]
[1 2 3]
```

- [ ] d. 
```
[1 2 3]
[1 2 3]
```

---

Язык С++. Что будет напечатано в результате исполнения следующего кода?

```cpp
#include <iostream>

class A{
public:
    void get(){
        std::cout << 'A';
    }
};

class B: public A{
public:
    void get(){
        std::cout << 'B';
    }
};

class C: public B{
public:
    void get(){
        std::cout << 'C';
    }
};

int main(){
    A* obj = new C;
    obj->get();
}
```

Ответ:
- [ ] a. B
- [x] b. A
- [ ] c. Ошибка
- [ ] d. C

---

Язык Go. Дан фрагмент кода:

```go
type A struct{
    value int
}

type Printer interface{
    print()
}

func main() {
    var obj A
    p := Printer(&obj)
    p.print()
}
```

Что нужно добавить, чтобы он стал рабочим?

Ответ:
- [ ] a. 
```go
func (a *A) Printer{
    fmt.Print(a.value)
}
```

- [x] b. 
```go
func (a *A) print(){
    fmt.Print(a.value)
}
```

- [ ] c. 
```go
func print(a *A){
    fmt.Print(a.value)
}
```

- [ ] d. Ничего. Код уже рабочий
- [ ] e. 
```go
func Printer(a *A){
    fmt.Print(a.value)
}
```

---

Дан фрагмент кода на С++. Что отобразится на экране после его выполнения?

```cpp
class Point{ 
};
 
int main(){
    Point p;
    std::cout << sizeof(p);
}
```

Ответ:
- [ ] a. 8
- [ ] b. 2
- [ ] c. 4
- [ ] d. Ошибка. Отсутствует конструктор по умолчанию
- [x] e. 1
- [ ] f. 0

---

Дан фрагмент кода на языке С++. Что будет на экране в результате выполнения данного фрагмента?  

```cpp
class SomeClass{
public:
    int value = 1;
    void method(int value){
        value = value;
    }
} s;

s.method(5);
std::cout << s.value;
```

Ответ:
- [x] a. Ошибка  
- [ ] b. 5  
- [ ] c. 1  

---

Язык Go. Что будет выведено на экран в результате работы этого кода?  

```go
package main
import "fmt"

type Data struct{
    i int
}

func (d Data) method(){
    d.i = 1
}

func main() {
    var d *Data = &Data{}
    d.method()
    fmt.Print(d.i)
}
```

Ответ:  
- [ ] a. Мусорное значение  
- [ ] b. Ошибка  
- [ ] c. 1  
- [x] d. 0  

---

Дан фрагмент кода на С++.  

```cpp
class SomeClass{  
    int value = 0;  
public:  
    void method() const{  
        value = 1;  
        std::cout << value;  
    }  
};  

int main(){  
    const SomeClass s;  
    s.method();  
}
```

Что нужно добавить в код, чтобы код стал рабочим?  

Ответ:  
- [ ] a. Добавить ключевое слово public к объявлению value;  
- [ ] b. Ничего добавлять не нужно. Код и так рабочий;  
- [x] c. Добавить ключевое слово mutable к объявлению value;  
- [ ] d. В константных методах нельзя менять значения полей и это изменить невозможно.  
- [ ] e. Добавить ключевое слово mutable к объявлению method;

---

Дан фрагмент кода на С++. Каким образом можно получить доступ к полю x переменной p?  

```cpp
struct Point{  
    double x, y;  
} p;
```

Ответ:  
- [ ] a. p->x;  
- [ ] b. p>x;  
- [x] c. p.x;  
- [ ] d. p-x;  
- [ ] e. p<-x;  
- [ ] f. p~x;

---

Дан фрагмент кода на С++. Что будет содержать переменная container после его выполнения?  

```cpp
std::map<std::string, int> container{{"1", 2}, {"3", 2}};  
container[-1] = 5;
```

Ответ:  
- [ ] a. Пары: 1:2, 3:2, -1:5  
- [ ] b. Пары: 1:2, 3:2, 5:-1  
- [ ] c. Пары: -1:5, 1:2, 3:2  
- [x] d. Ошибка компиляции - недопустимый тип  
- [ ] e. Ошибка доступа - указанного элемента не существует

---

Дан фрагмент кода на Go. Каким образом можно получить доступ к полю `x` переменной `var p Point = Point{}`?

```go
type Point struct{
    x float64
    y float64
}
```

Ответ:
- [x] a. 
```go
fmt.Print( p.x )
```

- [ ] b. 
```go
fmt.Print( *p.x )
```

- [x] c. 
```go
fmt.Print( (&p).x )
```

- [ ] d. 
```go
fmt.Print( (*p).x )
```

- [ ] e. 
```go
fmt.Print( p->x )
```

- [ ] f. 
```go
fmt.Print( &p.x )
```

---

Дан фрагмент кода на С++. Что будет содержать переменная container после его выполнения?  

```cpp
std::unordered_set<int> container{1, 2, 3, 4};  
container.insert(container.begin(), 4);
```

Ответ:  
- [ ] a. 1, 2, 3, 4, 4  
- [ ] b. 1, 2, 3, 4  
- [ ] c. 4, 1, 2, 3, 4  
- [x] d. 1, 2, 3, 4 но в порядке зависящем от хэш-функции

---

Дан фрагмент кода на С++. Что будет содержать переменная container2 после его выполнения?

```cpp
std::set<int> container1{4, 3, 2, 1, 2, 3, 4};  
std::vector<int> container2(container1.begin(), container1.end());
```

Ответ:  
- [ ] a. 1, 2, 3, 4 но в порядке зависящем от хэш-функции  
- [ ] b. 4, 3, 2, 1, 2, 3, 4  
- [ ] c. 4, 3, 2, 1, 2, 3, 4 но в порядке зависящем от хэш-функции  
- [ ] d. 4, 3, 2, 1  
- [x] e. 1, 2, 3, 4

---

Дан фрагмент кода на С++ и класс MyClass объявленный как:

```cpp
class MyClass{};
```


Выберите все верные варианты, которые являются допустимыми объявлениям.

Ответ:  
- [ ] a. double operator# (MyClass a, double b);  
- [x] b. double operator/ (MyClass a, double b);  
- [ ] c. double operator** (MyClass a, double b);  
- [x] d. double operator- (MyClass a, double b);  
- [x] e. double operator+ (MyClass a, double b);

---

Дан фрагмент кода на языке С++. Что выведется на экран в результате его работы?

```cpp
enum class Color {
    YELLOW,
    BLACK,
    PINK,
    GREEN
};

Color color = Color::BLACK;
std::cout << color;
```

Ответ:  
- [ ] a. Какое-то "мусорное" значение, т.к. для BLACK НЕ указано значение явно  
- [x] b. Ошибка. Для Color не определён оператор <<  
- [ ] c. 0  
- [ ] d. 1  
- [ ] e. Строка BLACK  

---

Дан код на Go. Что будет выведено на экран в результате его выполнения:

```go
package main
import "fmt"

func change(abc []int) {
    abc = append(abc, 4)
    for i := range abc {
        abc[i] = 4
    }
    fmt.Println(abc)
}

func main() {
    abc := []int{1, 2, 3}
    change(abc)
    fmt.Println(abc)
}
```

Ответ:  
- [ ] a. [4 4 4 4]  
  [4 4 4 4]  
- [x] b. [4 4 4 4]  
  [1 2 3]  
- [ ] c. [1 2 3]  
  [4 4 4 4]  
- [ ] d. [4 4 4 4]  
  [1 2 3 4]

---

Дан фрагмент кода на Go. Что будет содержать переменная container после его выполнения?

```go
var container map[string]int
container[-1] = 1
```

Ответ:  
- [ ] a. Ключ: -1 со значением: 1  
- [ ] b. Ключ: -1 со значением: 0  
- [x] c. Ошибка компиляции - недопустимый тип  
- [ ] d. Ошибка во время исполнения - под словарь не выделена память  
- [ ] e. Ошибка во время исполнения - попытка доступа к несуществующему ключу  

---

Язык С++. Что будет напечатано в результате исполнения следующего кода?

```cpp
#include <iostream>

class A{
public:
    virtual void get(){
        std::cout << 'A';
    }
};

class B: public A{
public:
    void get(){
        std::cout << 'B';
    }
};

class C: public B{
public:
    void get(){
        std::cout << 'C';
    }
};

int main(){
    A* obj = new C;
    obj->get();
}
```

Ответ:  
- [ ] a. Ошибка  
- [ ] b. A  
- [x] c. C  
- [ ] d. B  

---

Язык Go. Дан фрагмент кода:

```go
type A struct{
    value int
}

func (a A) get() int{
    return a.value
}

func (a A) set(val int){
    a.value = val
}

type Accessor interface{
    get() int
    set(val int)
}

func main() {
    var obj A
    g := Accessor(obj)
    g.set(10)
    
    fmt.Print(obj.get())
}
```

Ответ:  
- [x] a. 0  
- [ ] b. 10  
- [ ] c. Ошибка

---

Дан фрагмент кода на С++. Что отобразится на экране после его выполнения?

```cpp
class Point{
    Point(){}
};
 
int main(){
    Point p;
    std::cout << sizeof(p);
}
```

Ответ:  
- [x] a. Ошибка. Не доступен конструктор по умолчанию  
- [ ] b. 4  
- [ ] c. 8  
- [ ] d. 1  
- [ ] e. 2  
- [ ] f. 0

---

Дан фрагмент кода на языке С++. Что будет на экране в результате выполнения данного фрагмента?

```cpp
class SomeClass{
    int value = 1;
    void method(int value){
        value = value;
    }
} s;

s.method(5);
std::cout << s.value;
```

Ответ:  
- [ ] a. 1  
- [ ] b. 5  
- [x] c. Ошибка

---

Язык Go. Что будет выведено на экран в результате работы этого кода?

```go
package main
import "fmt"

type Data struct{
    i int
}

func (d *Data) method(){
    d.i = 1
}

func main() {
    var d Data = Data{}
    d.method()
    fmt.Print(d.i)
}
```

Ответ:  
- [ ] a. 0  
- [ ] b. Мусорное значение  
- [x] c. 1  
- [ ] d. Ошибка

---

Какое, из перечисленных, ключевых слов нужно использовать при объявлении структуры в С++?

Ответ:  
- [ ] a. object  
- [ ] b. record  
- [x] c. struct  
- [ ] d. Struct  
- [ ] e. Record  
- [ ] f. structure

---

Дан код на языке Go:

```cpp
#include <iostream>
#include <map>

void add(std::map<int, int> c, int value){
    c[c.size()] = value;
}

int main(){
    std::map<int, int> container;
    add(container, 10);
    std::cout << container.size();
}
```

Ответ:
- [x] a. Переменная container не изменится;
- [ ] b. Переменная container изменится;

---

Дан фрагмент кода на Go. Каким образом можно получить доступ к полю x переменной `var p *Point = &Point{}`?

```go
type Point struct{
    x float64
    y float64
}
```

Ответ:  
- [x] a. fmt.Print( p.x )  
- [ ] b. fmt.Print( *p.x )  
- [x] c. fmt.Print( (*p).x )  
- [ ] d. fmt.Print( (&p).x )  
- [ ] e. fmt.Print( &p.x )  
- [ ] f. fmt.Print( p->x )

---

Дан фрагмент кода на С++. Что будет содержать переменная container2 после его выполнения?

```cpp
std::vector<int> container1{4, 3, 2, 1, 2, 3, 4};
std::set<int> container2(container1.begin(), container1.end());
```

Ответ:  
- [ ] a. 4, 3, 2, 1, 2, 3, 4  
- [x] b. 1, 2, 3, 4  
- [ ] c. 4, 3, 2, 1, 2, 3, 4 но в порядке зависящем от хэш-функции  
- [ ] d. 4, 3, 2, 1  
- [ ] e. 1, 2, 3, 4 но в порядке зависящем от хэш-функции

---

Дан фрагмент кода на С++. Что будет содержать переменная container2 после его выполнения?

```cpp
std::vector<int> container1{4, 3, 2, 1, 2, 3, 4};
std::vector<int> container2(container1.begin(), container1.end());
```

Ответ:  
- [x] a. 4, 3, 2, 1, 2, 3, 4  
- [ ] b. 4, 3, 2, 1, 2, 3, 4 но в порядке зависящем от хэш-функции  
- [ ] c. 4, 3, 2, 1  
- [ ] d. 1, 2, 3, 4  
- [ ] e. 1, 2, 3, 4 но в порядке зависящем от хэш-функции

---

Дан фрагмент кода на С++ и класс `MyClass` объявленный как:

```cpp
class MyClass{};
```

Выберите все верные варианты, которые являются допустимыми объявлениями.

Ответ:
- [x] a. 
```cpp
double operator/ (MyClass a, double b);
```

- [ ] b. 
```cpp
double operator/ (MyClass a, double b, MyClass c);
```

- [x] c. 
```cpp
void operator/ (MyClass a, double b);
```

- [ ] d. 
```cpp
double operator/ (MyClass a);
```

- [ ] e. 
```cpp
double operator/ (MyClass a, MyClass b, double c);
```

---

Дан фрагмент кода на языке С++. Что выведется на экран в результате его работы?

```cpp
enum Color {
    YELLOW,
    BLACK,
    PINK,
    GREEN
};

int BLACK = 0;
Color color = Color::BLACK;
std::cout << color;
```

Ответ:
- [ ] a. 1  
- [ ] b. Строка BLACK  
- [ ] c. 0  
- [ ] d. Ошибка. Для Color не определён оператор <<  
- [x] e. Ошибка. Повторное объявление имени BLACK   (Важно! При условии если `int BLACK = 0;` считается глобальной переменной)

---

Дан код на Go. Что будет выведено на экран в результате его выполнения:

```go
package main
import "fmt"

func change(abc []int) {
    abc = append(abc, 4)
    for i := range abc {
        abc[i] = 4
    }
    fmt.Println(abc)
}

func main() {
    abc := []int{1, 2, 3}
    change(abc)
    abc = append(abc, 4)
    fmt.Println(abc)
}
```

Ответ:
- [ ] a. [1 2 3 4] [4 4 4 4]  
- [x] b. [4 4 4 4] [1 2 3 4]  
- [ ] c. [4 4 4 4] [4 4 4 4]  
- [ ] d. [1 2 3] [4 4 4 4 4]

---

Дан фрагмент кода на Go. Что будет содержать переменная container после его выполнения?

```go
var container map[int]int = make(map[int]int)
container[-1] = 1
```

Ответ:
- [x] a. Ключ: -1 со значением: 1  
- [ ] b. Ошибка компиляции - недопустимый тип  
- [ ] c. Ошибка во время исполнения - попытка доступа к несуществующему ключу  
- [ ] d. Ошибка во время исполнения - под словарь не выделена память  
- [ ] e. Ключ: -1 со значением: 0

---

Язык С++. Что будет напечатано в результате исполнения следующего кода?

```cpp
#include <iostream>

class A{
public:
    void get(){
        std::cout << 'A';
    }
};

class B: public A{
public:
    virtual void get(){
        std::cout << 'B';
    }
};

class C: public B{
public:
    virtual void get(){
        std::cout << 'C';
    }
};

int main(){
    A* obj = new C;
    obj->get();
}
```

Ответ:
- [ ] a. C
- [ ] b. B
- [x] c. A
- [ ] d. Ошибка

---

Язык Go. Дан фрагмент кода:

```go
type A struct{
    value int
}

func (a *A) get() int{
    return a.value
}

func (a *A) set(val int){
    a.value = val
}

type Accessor interface{
    get() int
    set(val int)
}

func main() {
    var obj A
    g := Accessor(&obj)
    g.set(10)
    
    fmt.Print(obj.get())
}
```

Что будет выведено в результате его работы?

Ответ:
- [ ] a. Ошибка  
- [ ] b. 0  
- [x] c. 10

---

Дан фрагмент кода на С++. Что отобразится на экране после его выполнения?

```cpp
class Point{
    char i;
public:
    Point(char val):i(val){}
};
 
int main(){
    Point p;
    std::cout << sizeof(p);
}
```

Ответ:
- [ ] a. 0  
- [ ] b. 8  
- [ ] c. 1  
- [ ] d. 4  
- [x] e. Ошибка. Отсутствует конструктор по умолчанию  
- [ ] f. 2

---

Дан фрагмент кода на языке С++. Что будет на экране в результате выполнения данного фрагмента?

```cpp
class SomeClass{
public:
    int value = 1;
    void method(int value){
        this->value = value;
    }
} s;

s.method(5);
std::cout << s.value;
```

Ответ:
- [ ] a. 1  
- [x] b. 5  
- [ ] c. Ошибка

---

Язык Go. Что будет выведено на экран в результате работы этого кода?

```go
package main
import "fmt"

type Data struct{
    i int
}

func (d *Data) method(){
    d.i = 1
}

func (d Data) method(){
    d.i = 1
}

func main() {
    var d *Data = &Data{}
    d.method()
    fmt.Print(d.i)
}
```

Ответ:
- [x] a. Ошибка
- [ ] b. 0
- [ ] c. Мусорное значение
- [ ] d. 1

---


Где можно объявить структуру в языке С++?

Ответ:
- [x] a. Внутри других структур;
- [ ] b. Там, где указан возвращаемый тип функции;
- [x] c. Вне функции;
- [x] d. Внутри тела функции;
- [ ] e. Внутри списка параметров функции;

---

Дан фрагмент кода на С++. Что будет содержать переменная container после его выполнения?

```cpp
std::map<int, int> container;
int a = container[-1];
```

Ответ:
- [ ] a. Ошибка во время исполнения - попытка доступа к несуществующему ключу
- [ ] b. Ошибка во время исполнения - под словарь не выделена память
- [ ] c. Ключ: -1 со значением: 1
- [ ] d. Ошибка компиляции - недопустимый тип
- [x] e. Ключ: -1 со значением: 0

---

Дан фрагмент кода на С++. Что будет содержать переменная `container2` после его выполнения?

```cpp
std::vector<int> container1{4, 3, 2, 1, 2, 3, 4};
std::unordered_set<int> container2(container1.begin(), container1.end());
```

Ответ:
- [ ] a. 1, 2, 3, 4
- [ ] b. 4, 3, 2, 1, 2, 3, 4
- [x] c. 1, 2, 3, 4 но в порядке зависящем от хэш-функции
- [ ] d. 4, 3, 2, 1, 2, 3, 4 но в порядке зависящем от хэш-функции
- [ ] e. 4, 3, 2, 1

---

**Какое, из перечисленных, ключевых слов нужно использовать при объявлении структуры в Go?**

Ответ:
- [x] a. struct
- [ ] b. Record
- [ ] c. Struct
- [ ] d. structure
- [ ] e. record
- [ ] f. object

---

Дан фрагмент кода на С++. Что будет содержать переменная `container` после его выполнения?

```cpp
std::vector<int> container{1, 2, 3, 2};
container[-1] = 5;
```

Ответ:
- [ ] a. Пары: 1:2, 3:2, -1:5
- [x] b. Ошибка доступа - указанного элемента не существует
- [ ] c. Ошибка компиляции - недопустимый тип
- [ ] d. Пары: 1:2, 3:2, 5:-1
- [ ] e. Пары: -1:5, 1:2, 3:2

---

Дан фрагмент кода на языке С++. Что выведется на экран в результате его работы?

```cpp
enum Color {
    YELLOW,
    BLACK = 0,
    PINK,
    GREEN
};

Color color = Color::BLACK;
std::cout << color;
```

Ответ:
- [ ] a. Строка BLACK
- [x] b. 0
- [ ] c. Ошибка. Перечислителям нельзя задавать значения явно
- [ ] d. Ошибка. Для Color не определён оператор <<
- [ ] e. 1

---

Дан код на Go. Что будет выведено на экран в результате его выполнения:

```go
package main
import "fmt"

func change(abc []int) {
    abc = append(abc, 4)
    for i := range abc {
        abc[i] = 4
    }
    fmt.Println(abc)
}

func main() {
    abc := []int{1, 2, 3}
    abc = append(abc, 4)
    change(abc)
    fmt.Println(abc)
}
```

Ответ:
- [ ] a. [1 2 3 4] [4 4 4 4 4]
- [ ] b. [4 4 4 4 4] [1 2 3 4]
- [ ] c. [4 4 4 4 4] [4 4 4 4 4]
- [x] d. [4 4 4 4 4] [4 4 4 4]

---

Дан код на языке Go:

```go
package main
import "fmt"

func add(c map[int]int, value int){
    c[len(c)] = value
}

func main() {
    var container map[int]int = make(map[int]int)
    add(container, 10)
    fmt.Print(len(container))
}
```

Ответ:
- [x] a. Переменная container изменится;
- [ ] b. Переменная container не изменится;

---

Дан код на языке C++:

```cpp
#include <iostream>

class A{
public:
    void get(){
        std::cout << 'A';
    }
};

class B: public A{
public:
    virtual void get(){
        std::cout << 'B';
    }
};

class C: public B{
public:
    virtual void get(){
        std::cout << 'C';
    }
};

int main(){
    C* obj = new A;
    obj->get();
}
```

Ответ:
- [x] a. Ошибка
- [ ] b. C
- [ ] c. A
- [ ] d. B

---

Дан фрагмент кода на языке Go:

```go
package main
import "fmt"

type A struct{
    value int
}

func (a *A) print(){
    fmt.Print("*")
}

type Printer interface{
    print()
}

func main() {
    var obj *A = nil
    p := Printer(obj)
    
    if p == nil{
        fmt.Print("+")
    } else {
        fmt.Print("-")
    }
}
```

Ответ:
- [ ] a. *
- [ ] b. Ошибка
- [ ] c. 0
- [x] d. -
- [ ] e. +

---

Дан фрагмент кода на С++:

```cpp
class MyClass{
public:
    double i = 1.0;
};

double operator+ (MyClass a, double b){
    return a.i + b;
}

MyClass obj;
double value = 2.0;
```
Выберите все выражения, которые НЕ вызовут ошибки:

Ответ:
- [ ] a. obj + obj;
- [x] b. obj + 2.0;
- [x] c. obj + value;
- [x] d. value + value;
- [ ] e. value + obj;

---

Дан фрагмент кода на С++:

```cpp
class Point{
    char i;
public:
    Point(char val=0):i(val){}
};

int main(){
    Point p;
    std::cout << sizeof(p);
}
```

Ответ:
- [ ] a. 4
- [ ] b. 2
- [ ] c. 0
- [ ] d. Ошибка. Отсутствует конструктор по умолчанию
- [x] e. 1
- [ ] f. 8

---

Дан фрагмент кода на С++:

```cpp
class SomeClass{
    int value = 0;
public:
    void one(){
        value = 1;
    }
    void one() const{
        std::cout << value;
    }

    void two(){
        value = 2;
    }

    void three() const {
        std::cout << 3;
    }

    void four() {
        std::cout << 4;
    }
};
```
и переменная созданная таким образом:
```cpp
const SomeClass s;
```
Какие из перечисленных вызовов метода НЕ приведут к ошибке?

Ответ:
- [ ] a. s.four();
- [ ] b. s.two();
- [x] c. s.one();
- [x] d. s.three();

---

Каким образом можно создать переменную типа структура в языке С++?

Ответ:
- [ ] a. 
```cpp
struct Point{
    int x,y;
};

Point;
```
- [x] b.
```cpp
struct Point{
    int x,y;
};

Point p1, p2;
```
- [x] c.
```cpp
struct{
    int x,y;
} p1, p2;
```
- [x] d.
```cpp
struct Point{
    int x,y;
} p1, p2;
```
- [ ] e.
```cpp
p1, p2: struct Point{
    int x,y;
};
```
- [ ] f.
```cpp
struct Point (p1, p2){
    int x,y;
};
```

---

Дан фрагмент кода на языке С++. Что выведется на экран в результате его работы?

```cpp
enum Color {
    YELLOW = 1,
    BLACK = 1,
    PINK,
    GREEN
};

Color color = Color::BLACK;
std::cout << color;
```

Ответ:
- [ ] a. 0
- [ ] b. Ошибка. Для Color не определён оператор <<
- [ ] c. Ошибка. Перечислителям нельзя задавать одинаковые значения
- [x] d. 1
- [ ] e. Строка BLACK

---

Дан фрагмент кода на языке Go. В текущий пакет из пакета other была импортирована структура:

```go
type Book struct {
    Title       string
    author      string
    Description string
    Price       int
    pages       int
}
```

Какие поля будут доступны для использования в текущем пакете?

Ответ:
- [x] a. Description
- [x] b. Price
- [ ] c. author
- [x] d. Title
- [ ] e. pages

---

Дан фрагмент кода на Go. Что будет содержать переменная `container` после его выполнения?

```go
var container map[int]int = make(map[int]int)
var a = container[-1]
```

Ответ:
- [ ] a. Ошибка во время исполнения - попытка доступа к несуществующему ключу
- [ ] b. Ключ: -1 со значением: 1
- [ ] c. Ошибка компиляции - недопустимый тип
- [x] d. Ключ: -1 со значением: 0
- [ ] e. Ошибка во время исполнения - под словарь не выделена память

---

Дан фрагмент кода на языке С++. Что будет напечатано в результате его исполнения?

```cpp
#include <iostream>

class A{
    char value = 'a';
public:
    virtual void get(){
        std::cout << value;
    }
};

class B: public A{
    char value = 'b';
public:
    virtual void get(){
        std::cout << value;
    }
};

void print(A* obj){
    obj->get();
}

int main(){
    print(new B);
}
```

Ответ:
- [x] a. b
- [ ] b. Ошибка
- [ ] c. a

---

Дан фрагмент кода на С++:

```cpp
class MyClass{
    double i = 1.0;
};

double operator+ (MyClass a, double b){
    return a.i + b;
}

MyClass obj;
double value = 2.0;
```
Выберите все выражения, которые НЕ вызовут ошибки:

Ответ:
- [x] a. value + value;
- [ ] b. obj + 2.0;
- [x] c. value + obj;
- [ ] d. obj + value;
- [ ] e. obj + obj;

---

Дан фрагмент кода на С++:

```cpp
class Point{
    char i;
public:
    Point():i(0){}
    Point(char val=0):i(val){}
};

int main(){
    Point p;
    std::cout << sizeof(p);
}
```

Ответ:
- [ ] a. 0
- [ ] b. 8
- [ ] c. 1
- [x] d. Ошибка. Двусмысленность при вызове конструктора
- [ ] e. 4
- [ ] f. 2

---

Язык С++. Дан класс:

```cpp
class SomeClass{
    int non_static_value = 0;
    static const int static_value = 0;
public:
    static void static_method() {
    }
    static void other_static_method() {
    }    

    void non_static_method() {
    }
    void other_non_static_method() {
    }
};
```

Ответ:
- [x] a. В `static_method` можно использовать `static_value`;
- [ ] b. В `static_method` можно использовать `non_static_method`;
- [x] c. В `static_method` можно использовать `other_static_method`;
- [ ] d. В `static_method` можно использовать `non_static_value`;
- [ ] e. Чтобы вызвать `static_method` обязательно нужно создать объект;
- [x] f. Чтобы вызвать `static_method` НЕ обязательно иметь объект;

---

Язык С++. Что будет напечатано в результате исполнения следующего кода?

```cpp
#include <iostream>

class A{
    char value = 'a';
public:
    void get(){
        std::cout << value;
    }
};

class B: public A{
    char value = 'b';
public:
    void get(){
        std::cout << value;
    }
};

void print(A* obj){
    obj->get();
}

int main(){
    print(new B);
}
```

Ответ:
- [ ] a. b
- [x] b. a
- [ ] c. Ошибка

---

Язык С++. Дан класс:

```cpp
class SomeClass{
    int non_static_value = 0;
    static const int static_value = 0;
public:
    static void static_method() {
    }
    static void other_static_method() {
    }    

    void non_static_method() {
    }
    void other_non_static_method() {
    }
};
```

Выберите все верные утверждения.

Ответ:
- [x] a. В `non_static_method` можно использовать `non_static_value`;
- [x] b. Чтобы вызвать `non_static_method` обязательно нужно создать объект;
- [ ] c. Чтобы вызвать `non_static_method` НЕ обязательно иметь объект;
- [x] d. В `non_static_method` можно использовать `other_non_static_method`;
- [x] e. В `non_static_method` можно использовать `static_value`;
- [x] f. В `non_static_method` можно использовать `static_method`;

---

Язык С++. Дана структура:

```cpp
struct Point{
    int x, y;
};
```

Каким образом можно создать переменную типа `Point`?

Ответ:
- [ ] a. `Point(1, 2) p;`
- [x] b. `Point p = {};`
- [ ] c. `Point p();`
- [x] d. `Point p {1, 2};`
- [x] e. `Point p = {1, 1};`
- [ ] f. `Point p = [1, 1];`
- [ ] g. `Point p {x=1, y=1};`

---

Язык С++. Что из перечисленного может быть использовано в качестве объявления конструктора по умолчанию для класса `SomeClass`?

Ответ:
- [x] a. `SomeClass() = default;`
- [x] b. `SomeClass(){}`
- [ ] c. `SomeClass(int a) = default;`
- [ ] d. `default SomeClass(){}`
- [x] e. `SomeClass(int a=0, int b=0){}`
- [ ] f. `SomeClass(int a) {}`

---

Дан фрагмент кода на языке С++. Что выведется на экран в результате его работы?

```cpp
enum Color {
    YELLOW = -2,
    BLACK,
    PINK = -1,
    GREEN = 0
};

Color color = Color::BLACK;
std::cout << color;
```

Ответ:
- [ ] a. -3
- [x] b. -1
- [ ] c. Ошибка. Всем перечислителям должны быть заданы значения
- [ ] d. Ошибка. Для Color не определён оператор <<
- [ ] e. 1

---

Язык Go. Структуры A, B и C и переменные a, b, c объявлены следующим образом:

```go
type A struct {
    Title       string
    Author      string
}

type B struct {
    Title       string
    Author      string
}

type C = struct {
    Title       string
    Author      string
}

a := A{}
b := B{}
c := C{}
```

Ответ:
- [x] a. a = c
- [ ] b. b = a
- [x] c. c = a
- [ ] d. a = b
- [x] e. b = c
- [x] f. c = b

---

Дан фрагмент кода на С++:

```cpp
class MyClass{
    double i = 1.0;
    friend double operator+ (MyClass a, double b);
};

double operator+ (MyClass a, double b){
    return a.i + b;
}

MyClass obj;
double value = 2.0;
```

Ответ:
- [x] a. obj + 2.0;
- [x] b. obj + value;
- [x] c. value + value;
- [ ] d. obj + obj;
- [ ] e. value + obj;

---

Язык С++. Что из нижеперечисленного может быть полем структуры?

Ответ:

- [x] a. Массивы;
- [x] b. Указатели;
- [x] c. Строки;
- [x] d. Структуры и Классы;
- [x] e. Фундаментальные типы (int, double, ...);

---

Дан фрагмент кода на языке С++. Выберите верные утверждения.

```cpp
enum Color {
    YELLOW = -2,
    BLACK,
    PINK = -2,
    GREEN
};
```

Ответ:

- [x] a. GREEN равно -1
- [ ] b. GREEN и BLACK содержат мусорные значения
- [x] c. BLACK равно -1
- [ ] d. Ошибка. Дублирование значений.
- [ ] e. BLACK равно -2
- [ ] f. GREEN равно -2

---

Язык Go. Какая структура называется анонимной?

Ответ:
- [x] a. 
```go
var a struct {
    Title     string
    Author    string
}
```

- [ ] b. В Go нет анонимных структур
- [ ] c. 
```go
type anonymous struct {
    Title     string
    Author    string
}
```

- [ ] d. 
```go
type {
    Title     string
    Author    string
}
```

- [ ] e. 
```go
type struct {
    Title     string
    Author    string
}
```

---

Язык С++. Для чего используется ключевое слово `override`?

Ответ:
- [x] a. Это ключевое слово НЕ обязательно использовать. Оно заставляет компилятор проверить, что у предка существует такой же метод как текущий и что он виртуальный. Если это не так, будет ошибка компиляции;
- [ ] b. Это ключевое слово ни на что не влияет и используется как своего рода комментарий, который показывает, что текущий метод переопределяет метод предка;
- [ ] c. Это ключевое слово обязательно должно быть использовано если текущий метод переопределяет метод предка. Иначе будет ошибка компиляции;
- [ ] d. Это ключевое слово используется в классе-предке, чтобы показать, что текущий метод необходимо переопределить в наследнике. Если в наследнике не будет такого метода, то компилятор кинет ошибку;
- [ ] e. Если метод наследника отмечен этим ключевым словом, то он будет переопределять метод предка. Если не отмечен, то переопределения не будет;

---

Дан фрагмент кода на С++. Каким образом можно создать один или несколько объектов класса `Point`?

```cpp
struct Point{
    Point(){}
};
```


Ответ:
- [ ] a. 
```cpp
Point p();
```

- [x] b. 
```cpp
new Point();
```

- [x] c. 
```cpp
class Point p;
```

- [x] d. 
```cpp
new Point;
```

- [x] e. 
```cpp
Point p;
```

- [x] f. 
```cpp
Point p[10];
```

---

Дан фрагмент кода на С++. Что будет на экране в результате выполнения данного фрагмента?

```cpp
class MyClass{
public:
    double i = 1.0;
};

MyClass obj;
if (obj) std::cout << 1;
else std::cout << 2;
```

Ответ:
- [ ] a. 2
- [ ] b. 1
- [x] c. Ошибка

---

Язык С++. Дана структура:

```cpp
struct Point{
    int x,y;
};
```

Выберите фрагменты, не вызывающие ошибки.

Ответ:
- [x] a. 
```cpp
Point p = {1, 1}, p2;
p2 = p;
```

- [x] b. 
```cpp
Point p;
p = {1, 1};
```

- [x] c. 
```cpp
Point p = {1, 1}, p2;
p2 = {p.y, p.x};
```

- [x] d. 
```cpp
Point p = {1, 1}, p2 = {2, 2};
p2.x = p2.x + p.x;
p2.y = p2.y + p.y;
```

- [ ] e. 
```cpp
Point p = {1, 1}, p2 = {2, 2};
p2 = p2 + p;
```

---

Дан фрагмент кода на языке С++. Выберите верные утверждения.

```cpp
enum Color {
    YELLOW = -1,
    BLACK = 0,
    PINK = 1,
    GREEN = 2
};
```

Ответ:
- [x] a. 
```cpp
int i = YELLOW + PINK; // i будет равно 0
```

- [ ] b. Во всех остальных вариантах будет ошибка, т.к. для Color не определены операторы + и/или =
- [x] c. 
```cpp
Color mix = Color(YELLOW + PINK); // mix будет равно BLACK
```

- [ ] d. 
```cpp
Color mix = YELLOW + PINK; // mix будет равно BLACK
```

---

Код на С++. Требуется запретить доступ к конструктору класса `Point` из внешнего кода, как это можно сделать?

Ответ:
- [ ] a. 
```cpp
private Point();
```

- [ ] b. 
```cpp
explicit Point();
```

- [ ] c. Никак
- [x] d. 
```cpp
private: Point();
```

- [ ] e. 
```cpp
__Point__();
```

- [ ] f. 
```cpp
~Point();
```

---

Язык Go. Дан тип данных и функция:

```go
type Point struct {
    x, y int
}

func foo(p Point){
    p.x = 10
    p.y = 10
}
```

Если передать переменную типа `Point` в функцию `foo` указанным выше образом, то:

Ответ:
- [ ] a. Ошибка. Т.к. структуры нельзя передавать в функции;
- [x] b. Внутри функции будет доступна копия структуры;
- [ ] c. Внутри функции будет создана пустая структура (т.е. поля содержат нули);
- [ ] d. Внутри функции будет доступен оригинал структуры;

---

Дан фрагмент кода на С++. Что будет на экране в результате выполнения данного фрагмента?

```cpp
class MyClass{
public:
    double i = 1.0;
    operator bool(){
        return i > 1.0;
    }
};

MyClass obj;
if (obj) std::cout << 1;
else std::cout << 2;
```

Ответ:
- [ ] a. 1
- [ ] b. Ошибка
- [x] c. 2

---

Язык С++. Какая структура называется анонимной?

Ответ:
- [ ] a. 
```cpp
struct anonymous{
    int x,y;
} s;
```

- [ ] b. 
```cpp
anonymous struct Point{
    int x,y;
} s;
```

- [ ] c. 
```cpp
struct Point{
    int x,y;
} anonymous;
```

- [x] d. 
```cpp
struct {
    int x,y;
} s;
```

- [ ] e. В С++ нет анонимных структур
- [ ] f. 
```cpp
struct Point{
    int x,y;
} s;
```

---

Дан фрагмент кода на языке С++. Выберите верные утверждения.

```cpp
enum class Color {
    YELLOW = -1,
    BLACK = 0,
    PINK = 1,
    GREEN = 2
};
```

Ответ:
- [x] a. Во всех остальных вариантах будет ошибка, т.к. для Color не определены операторы + и/или =
- [ ] b. 
```cpp
Color mix = Color::YELLOW + Color::PINK; // mix будет равно BLACK
```

- [ ] c. 
```cpp
int i = Color::YELLOW + Color::PINK; // i будет равно 0
```

- [ ] d. 
```cpp
Color mix = Color(Color::YELLOW + Color::PINK); // mix будет равно BLACK
```

---

Код на С++. Сколько конструкторов может быть у класса?

Ответ:
- [ ] a. Только 2
- [ ] b. Только 3
- [x] c. Сколько угодно
- [ ] d. Только 1

---

Дан фрагмент кода на С++. Что будет на экране в результате выполнения данного фрагмента?

```cpp
class MyClass{
public:
    double i = 1.0;
    void operator*(double b){
        i * b;
    }
};

MyClass obj;
std::cout << obj * 1.0;
```

Ответ:
- [ ] a. 1
- [x] b. Ошибка
- [ ] c. 2

---

Язык С++. Дан тип данных и прототип функции:

```cpp
struct Point{
    int x,y;
};

void foo(Point p);
```

Если передать переменную типа `Point` в функцию `foo` указанным выше образом, то:

Ответ:
- [ ] a. Ошибка. Т.к. структуры нельзя передавать в функции;
- [ ] b. Внутри функции будет доступен оригинал структуры;
- [ ] c. Внутри функции будет создана пустая структура (т.е. поля содержат "мусор");
- [x] d. Внутри функции будет доступна копия структуры;

---

Язык С++. Что из перечисленного является правильным объявлением и определением класса?

Ответ:
- [ ] a. 
```cpp
SomeClass class {};
```

- [ ] b. 
```cpp
struct SomeClass{}
```

- [x] c. 
```cpp
class SomeClass{};
```

- [x] d. 
```cpp
struct SomeClass{};
```

- [ ] e. 
```cpp
class SomeClass{}
```

- [ ] f. 
```cpp
class SomeClass: {};
```

---

Дан фрагмент кода на языке С++:

```cpp
enum class Color {
    YELLOW = 0,
    BLACK = 1,
    PINK = 2
};
Color color = Color::BLACK;
```

и функция с представленным ниже прототипом:

```cpp
void foo(Color clr);
```

Какой вариант вызова функции НЕ приведёт к ошибке?

Ответ:
- [ ] a. 
```cpp
foo(1);
```

- [ ] b. 
```cpp
foo("YELLOW");
```

- [x] c. 
```cpp
foo(Color(1));
```

- [x] d. 
```cpp
foo(color);
```

- [x] e. 
```cpp
foo(Color::YELLOW);
```

- [ ] f. 
```cpp
foo(YELLOW);
```

---

Дан фрагмент кода на С++. Что будет на экране в результате выполнения данного фрагмента?

```cpp
class MyClass{
public:
    double i = 1.0;
    double operator*(double b){
        return i * b;
    }
};

MyClass obj;
std::cout << obj * 1.0;
```

Ответ:
- [ ] a. 2
- [ ] b. Ошибка
- [x] c. 1

---

Дан фрагмент кода на языке С++:

```cpp
struct One{
    int x,y;
} a;

struct Two{
    int x,y;
} b;
```

Допустим ли следующий код?

```cpp
a = {1, 2};
b = a;
```

Ответ:
- [x] a. Нет, т.к. у a и b разные типы;
- [ ] b. Да, b получит копию полей a;
- [ ] c. Нет, для типа данных: структура, разрешено только почленное присваивание;
- [ ] d. Да, a и b будут ссылаться на одни и те же значения;

---

Язык С++. Где можно объявить/определить новый класс?

Ответ:
- [x] a. В теле функции
- [ ] b. В списке параметров функции
- [x] c. Внутри конструкции блок {}
- [x] d. В глобальной области видимости
- [x] e. Внутри других классов

---

Дан фрагмент кода на языке С++:

```cpp
enum Color {
    YELLOW = 0,
    BLACK = 1,
    PINK = 2
};
Color color = Color::BLACK;
```

и функция с представленным ниже прототипом:

```cpp
void foo(Color clr);
```

Какой вариант вызова функции НЕ приведёт к ошибке?

Ответ:
- [x] a. 
```cpp
foo(Color(1));
```

- [ ] b. 
```cpp
foo("YELLOW");
```

- [x] c. 
```cpp
foo(YELLOW);
```

- [x] d. 
```cpp
foo(Color::YELLOW);
```

- [ ] e. 
```cpp
foo(1);
```

- [x] f. 
```cpp
foo(color);
```

---

Дан фрагмент кода на языке С++:

```cpp
struct One{
    int x,y;
} a, b;
```

Допустим ли следующий код?

```cpp
a = {1, 2};
b = a;
```

Ответ:
- [ ] a. Нет, для типа данных: структура, разрешено только почленное присваивание;
- [ ] b. Да, a и b будут ссылаться на одни и те же значения;
- [x] c. Да, b получит копию полей a;
- [ ] d. Нет, т.к. у a и b разные типы;

---

Язык С++. Дано перечисление. Выберите все способы получения значения со стандартного ввода.

```cpp
enum Color {
    YELLOW = 0,
    BLACK = 1,
    PINK = 2
} color;
```

Ответ:
- [ ] a. 
```cpp
std::cin >> color; // Пользователь вводит строку: YELLOW
```

- [ ] b. 
```cpp
std::cin >> color; // Пользователь вводит число: 0
```

- [x] c. 
```cpp
int i;
std::cin >> i; // Пользователь вводит число: 0
color = Color(i);
```

- [ ] d. 
```cpp
std::string str;
std::cin >> str; // Пользователь вводит строку: YELLOW
color = static_cast<Color>(str);
```

- [x] e. 
```cpp
int i;
std::cin >> i; // Пользователь вводит число: 0
color = static_cast<Color>(i);
```

---

Дан фрагмент кода на языке С++. Что будет напечатано в результате его выполнения?

```cpp
struct SomeClass{
    int i = 0;
    int j = 0;
    
    SomeClass(){
        i = 10;
    }
    
    SomeClass(int value):SomeClass(){
        j = value;
    }
};

SomeClass obj(10);
std::cout << obj.i << ' ' << obj.j;
```

Ответ:
- [ ] a. 0 10
- [ ] b. 10 0
- [ ] c. 0 0
- [x] d. 10 10

---

Дан фрагмент кода на языке С++. Что будет напечатано в результате его выполнения?

```cpp
struct SomeClass{
    int i = 0;
    int j = 0;
    
    SomeClass(){
        i = 10;    
    }
    
    SomeClass(int value){
        SomeClass();
        j = value;
    }
};

SomeClass obj(10);
std::cout << obj.i << ' ' << obj.j;
```

Ответ:
- [ ] a. 10 0
- [ ] b. 10 10
- [x] c. 0 10
- [ ] d. 0 0

---

Дан фрагмент кода на языке С++. Что будет напечатано в результате его выполнения?

```cpp
struct SomeClass{
    int i = 0;
    
    SomeClass(int i){
        i = i;
    }
};

SomeClass obj(10);
std::cout << obj.i;
```

Ответ:
- [ ] a. Ошибка
- [x] b. 0
- [ ] c. 10

---

Дан фрагмент кода на языке С++. Что будет напечатано в результате его выполнения?

```cpp
struct SomeClass{
    int i = 0;
    
    SomeClass(int i){
        this->i = i;
    }
};

SomeClass obj(10);
std::cout << obj.i;
```

Ответ:
- [ ] a. 0
- [x] b. 10
- [ ] c. Ошибка

---

Дан фрагмент кода на языке С++. Что будет напечатано в результате его выполнения?

```cpp
struct SomeClass{
    int i = 0;
    
    SomeClass(int i):i(i){
    }
};

SomeClass obj(10);
std::cout << obj.i;
```

Ответ:
- [x] a. 10
- [ ] b. 0
- [ ] c. Ошибка

---

Язык С++. От чего зависит порядок инициализации полей класса, указанных в списке инициализаторов?

Ответ:
- [ ] a. От того, как какие у полей модификаторы доступа
- [x] b. От того, в каком порядке эти поля объявлены
- [ ] c. От того, в каком порядке эти поля перечислены в списке инициализаторов
- [ ] d. Порядок случайный

---

Дан фрагмент кода на языке С++. Что будет напечатано в результате его выполнения?

```cpp
struct SomeClass{
    const int i = 0;
    
    SomeClass(int i):i(i){
    }
};

SomeClass obj(10);
cout << obj.i;
```

Ответ:
- [ ] a. 0
- [x] b. 10
- [ ] c. Ошибка

---

Дан фрагмент кода на языке С++. Что будет напечатано в результате его выполнения?

```cpp
struct SomeClass{
    const int i = 0;
    
    SomeClass(int i){
        this->i = i;
    }
};

SomeClass obj(10);
std::cout << obj.i;
```

Ответ:
- [ ] a. 0
- [x] b. Ошибка
- [ ] c. 10

---

Дан фрагмент кода на языке С++. Что будет напечатано в результате его выполнения?

```cpp
struct SomeClass{
    int& i;
    
    SomeClass(int& i):i(i){
    }
};

int i = 0;
SomeClass obj(i);
std::cout << obj.i;
```

Ответ:
- [x] a. 0
- [ ] b. Ошибка
- [ ] c. 10

---

Дан фрагмент кода на языке С++. Что будет напечатано в результате его выполнения?

```cpp
struct SomeClass{
    int& i;
    
    SomeClass(int& i){
        this->i = i;
    }
};

int i = 0;
SomeClass obj(i);
std::cout << obj.i;
```

Ответ:
- [x] a. 0
- [ ] b. 10
- [ ] c. Ошибка

---

НОВАЯ!

Дан фрагмент кода на С++. Что отобразится на экране после его выполнения?

```cpp
class Point{
    char i;
public:
    Point(char val):i(val){}
};
 
int main(){
    Point p;
    std::cout << sizeof(p);
}
```
- [ ] a. 2
- [ ] b. 1
- [ ] c. 8
- [ ] d. 4
- [x] e. Ошибка. Отсутствует конструктор по умолчанию
- [ ] f. 0

---























