# 2023-LightDance-RPi

## playloop

### OF

```c++
struct Status {
	int r;
	int g;
	int b;
	int a;
};

vector<vector<Status>> frames;

class OpticalFiberController {
	public:
		OpticalFiberController();
		int init(vector<int> map);
		int sendAll(vector<Status> status); // lightall, darkall
};

// logic
while(1) {
	status = findFrame(getTime());
	sendAll(status);
	sleep(5ms);
}
```

### LED

```c++
struct Status {
	int r;
	int g;
	int b;
	int a;
};

vector<LEDStatus> stripStatus(size);

class LEDStripController {
	public:
		LEDStripController();
		int init(vector<int> shape);
		int checkReady();
		int sendAll(vector<vector<Status>> status);
};

// logic
while(1) {
	while(checkReady() == 0){
		sleep(1ms);
  }
	status = findFrame(getTime());
	sendAll(status);
	sleep(5ms);
}
```
