class MyHashMap {
    private:
        struct Node {
            int key, val;
            Node* next;
            Node(int k, int v) : key(k), val(v), next(nullptr) {}
        };
    
        static const int len = 1000;
        Node* buckets[len];
    
        Node* search(Node* head, int key) {
            Node* prev = head;
            Node* curr = head->next;
            while (curr != nullptr && curr->key != key) {
                prev = curr;
                curr = curr->next;
            }
            return prev;
        }
    
    public:
        MyHashMap() {
            for (int i = 0; i < len; ++i) {
                buckets[i] = nullptr;
            }
        }
    
        void put(int key, int val) {
            int idx = key % len;
            if (buckets[idx] == nullptr) {
                buckets[idx] = new Node(-1, -1); 
            }
            Node* prev = search(buckets[idx], key);
            if (prev->next == nullptr) {
                prev->next = new Node(key, val);
            } else {
                prev->next->val = val;
            }
        }
    
        int get(int key) {
            int idx = key % len;
            if (buckets[idx] == nullptr) return -1;
    
            Node* prev = search(buckets[idx], key);
            if (prev->next == nullptr) return -1;
    
            return prev->next->val;
        }
    
        void remove(int key) {
            int idx = key % len;
            if (buckets[idx] == nullptr) return;
    
            Node* prev = search(buckets[idx], key);
            if (prev->next == nullptr) return;
    
            Node* temp = prev->next;
            prev->next = prev->next->next;
            delete temp;
        }
    
    };