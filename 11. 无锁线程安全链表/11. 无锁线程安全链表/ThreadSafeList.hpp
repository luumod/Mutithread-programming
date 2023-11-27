
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
//
//class Test {
//public:
//	Test() {
//		
//	}
//
//	void test1() {
//		aic.store(3);
//
//		//返回true：如果当前值 aic 等于预期值 excepted_val，则更新 aic 为期望值 desired_val
//		//返回false：如果当前值 aic 不等于预期值 excepted_val，则更新预期值 excepted_val 为当前值 aic
//		if (aic.compare_exchange_weak(excepted_val,desired_val)) {
//			std::cout << "aic满足预期，更新为期望值\n";
//		}
//		else {
//			std::cout << "aic不满足预期，更新预期值\n";
//		}
//		std::cout << "aic: " << aic << ", excepted_val: " << excepted_val << ", dirsired_val: " << desired_val << '\n';
//	}
//private:
//	std::atomic<int> aic;
//	int excepted_val = 100;
//	int desired_val = 200;
//	bool excepted = false;
//};

//无头单链表
template <typename T>
class ThreadSafeList {
	struct Node {
		T val;
		std::atomic<Node*> prev;
		std::atomic<Node*> next;
		Node(const T& val) :val(val), next(nullptr) {}
		Node(const T& val,Node* prev=nullptr, Node* next=nullptr) :val(val),prev(prev),next(next) {}
	};
public:
	ThreadSafeList() {}
	void push_front(const T& val) {
		Node* new_node = new Node(val, tail.load(), head.load()); // create a new_node
		Node* current_head = head.load();    // temporary storage the head node

		//when list is empty. we need to process the head and the tail.
		if (!current_head) {
			tail.store(new_node);
		}
		do{
			//new_node->next = head
			new_node->next.store(current_head);
			//head = new_node
		} while (!head.compare_exchange_weak(current_head,new_node));

		//finished the prev pointer.
		if (new_node->next.load()) {
			new_node->next.load()->prev.store(new_node);
		}
	}
	void push_back(const T& val) {
		Node* new_node = new Node(val,tail.load(), nullptr); //create a new_node
		Node* current_tail = tail.load();	//temporary storage the tail node.

		//when list is empty, same with the push_front method.
		if (!current_tail) {
			push_front(val);
			return;
		}
		Node* temp = nullptr; //we need the nullptr node in the next operation.
		do {
			//makesure the current_tail is the tail node.
			current_tail = tail.load();
			//current_tail->next = new_node
		} while (!current_tail->next.compare_exchange_weak(temp, new_node));
		//tail = new_node
		if (current_tail->next.load()) {
			new_node->prev.store(current_tail);
			tail.compare_exchange_weak(current_tail, new_node);
		}

	}
	bool pop_front(T& val) {
		Node* current_head = head.load(); //temporary storage the head node.
		Node* new_head = nullptr;	//we need to pop head, so we need a new_head
		Node* temp = nullptr; //temporary storage for the next operation.
		do{
			//if head is nullptr, so the list is empty.
			//return false so the pop_front false.
			if (!current_head) {
				return false;
			}
			//if the list just have one node, so when we pop the head, we also need to handle the tail.
			if (current_head == tail.load()) { //head == tail
				//tail = nullptr
				tail.compare_exchange_weak(current_head, temp);
			}
			//new_head = nullptr 
			//new_head = the next node of head.
			new_head = current_head->next.load();
			//head = new_node
		} while (!head.compare_exchange_weak(current_head, new_head));

		if (new_head){
			new_head->prev.store(nullptr);
		}
		val = current_head->val;
		delete current_head;
		current_head = nullptr;
		return true;
	}
	//
	bool pop_back(T& val) {
		Node* current_tail = tail.load();
		Node* new_tail = nullptr, *temp = nullptr;
		do{
			if (!current_tail) {
				return false;
			}

			if (current_tail == head.load()) {
				head.compare_exchange_weak(current_tail, temp);
			}
			new_tail = current_tail->prev.load();
		} while (!tail.compare_exchange_weak(current_tail,new_tail));
		
		if (new_tail) {
			new_tail->next.store(nullptr);
		}
		val = current_tail->val;
		delete current_tail;
		current_tail = nullptr;
		return true;
	}

	friend std::ostream& operator<<(std::ostream& out, const ThreadSafeList& other) {
		Node* temp = other.head.load();
		while (temp) {
			out << temp->val << ' ';
			temp = temp->next;
		}
		return out;
	}
private:
	std::atomic<Node*> head;
	std::atomic<Node*> tail;
};

