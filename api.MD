# ArduinoMenu 5 API

## Types

`idx_t` item index/position (ie: `int`)

## Enumerations

## Navigation

### Static API

**Navigation functions**

```c++
idx_t pos()
```
get current/selected item position/index

```c++
idx_t size()
```
get current menu size (number of items)

```c++
bool selected(idx_t)
```
check if item is selected given its index

```c++
bool enabled(idx_t)
```
check if item is enabled given its index

```c++
void enable(idx_t n,bool o)
```
enable/disable indexed item (provided by enDis component)

```c++
template<typename Nav>
Modes mode(Nav& nav)
```
get current item editing mode

```c++
bool up()
```
increment selection index

```c++
bool down()
```
decrement selection index

```c++
bool left()
```
move selection left (not implemented yet, 2D Navigation)

```c++
bool right()
```
move selection right (not implemented yet, 2D Navigation)


```c++
bool enter()
```
activate current selection

```c++
bool esc()
```
exit current menu

```c++
NavAgent activate()
```
Activate selected item and obtain its navigation agent.

**Rendering functions**

```c++
template<typename Out>
void printMenu(Out& out)
```
print the associated menu structure into the specified output device

```c++
void enterMenuRender()
```
aux, function to control render behavior

```c++
void exitMenuRender()
```
aux, function to control render behavior

### Dynamic API

**virtual inline void printMenu(IMenuOut& out)**
print the associated menu structure into the specified output device

## Output

## Structure
