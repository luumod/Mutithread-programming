#include <iostream>
#include <thread>
#include <Windows.h>

void print1() {

	std::cout << "主线程\n";
}
void printtttt1() {
	Sleep(2000);
	std::cout << "子线程1\n";
}
void printtttt2() {
	Sleep(2000);
	std::cout << "子线程2\n";
}
void printtttt3() {
	Sleep(2000);
	std::cout << "子线程3\n";
}
int main() {
	std::thread t1(printtttt1);
	std::thread t2(printtttt2);
	std::thread t3(printtttt3);
	//t.join(); //子线程加入主线程
	//detach: 子线程和主线程各自玩个的，等待主线程执行完毕
	t1.detach();
	t2.detach();
	t3.detach();
	print1();
	return 0;
}