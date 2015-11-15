#ifndef __SKHOLE_SHAREDPTR__
#define __SKHOLE_SHAREDPTR__

#include <atomic>
#include <iostream>

namespace cs540 {

	// stores the counter for memory allocation.
	class RefCounter {

		template <typename T>
                friend class SharedPtr;

		private:
			std::atomic<int>* ref_count;

		public:
			RefCounter(){
				ref_count = new std::atomic<int>();
			}
			virtual ~RefCounter() {
				delete ref_count;
			}
	};

	// hold the actual data of type H.
	template <typename H>
	class RefObject : public RefCounter {

		private:
			H* object;
		public:
			RefObject(H* other) {
				object = other;
			}
			~RefObject() {
				delete object;
				object = nullptr;
			}
	};


	template <typename T>
	class SharedPtr {

		public:
			T* data;
			RefCounter* ref_obj;

			SharedPtr() : data(nullptr), ref_obj(nullptr) {

			}

			template <typename U>
			explicit SharedPtr(U* other) {
				ref_obj = new RefObject<U>(other);
				data = static_cast<T*>(other);
				++(*(ref_obj->ref_count));
			}

			SharedPtr(const SharedPtr& sp) {
				data = static_cast<T*>(sp.get());
                                ref_obj = sp.ref_obj;
                                if( ref_obj != nullptr){
                                        ++(*(ref_obj->ref_count));
				}
			}

			template <typename U>
			SharedPtr(const SharedPtr<U>& sp) {
				data = static_cast<T*>(sp.get());
                                ref_obj = sp.ref_obj;
                                if(ref_obj != nullptr){
                                        ++(*(ref_obj->ref_count));
                                }
			}

			SharedPtr(SharedPtr&& sp) {
                                        data = static_cast<T*>(sp.get());
                                        ref_obj = sp.ref_obj;
                                        sp.data = nullptr;
                                        sp.ref_obj = nullptr;
			}

			template <typename U>
			SharedPtr(SharedPtr<U> &&sp) {
				data = static_cast<T*>(sp.get());
				ref_obj = sp.ref_obj;
				sp.data = nullptr;
				sp.ref_obj = nullptr;
			}

			SharedPtr &operator=(SharedPtr &&sp) {
					std::cout << "inside move = \n";
					if(ref_obj != nullptr && --(*(ref_obj->ref_count)) == 0) {
						delete ref_obj;
						ref_obj = nullptr;
                                        	data = nullptr;
					}
                                        data = static_cast<T*>(sp.get());
                                        ref_obj = sp.ref_obj;
                                        sp.data = nullptr;
                                        sp.ref_obj = nullptr;
				return *this;
			}

			template <typename U>
			SharedPtr &operator=(SharedPtr<U> &&sp) {
				std::cout << "inside move = \n";
                                if(ref_obj != nullptr && --(*(ref_obj->ref_count)) == 0) {
                                        delete ref_obj;
                                        ref_obj = nullptr;
                                	data = nullptr;
                                }
                                data = static_cast<T*>(sp.get());
                                ref_obj = sp.ref_obj;
                                sp.data = nullptr;
                                sp.ref_obj = nullptr;
				return *this;
			}

			~SharedPtr(){
				if(ref_obj != nullptr && --(*(ref_obj->ref_count)) == 0){
					delete ref_obj;
					ref_obj = nullptr;
					data = nullptr;
				}
			}

			T* get() const {
				return data;
			}

			void reset() {
                                if(ref_obj != nullptr) {
                                        if(--(*(ref_obj->ref_count)) == 0) {
                                                delete ref_obj;
                                        }
                                        ref_obj = nullptr;
                                        data = nullptr;
                                }
                        }

			template <typename U>
			void reset(U *p) {
				if(ref_obj != nullptr ) {
					if(--(*(ref_obj->ref_count)) == 0) {
						delete ref_obj;
					}
					ref_obj = nullptr;
					data = nullptr;
				}
				ref_obj = new RefObject<U>(p);
				data = static_cast<U*>(p);
				++(*(ref_obj->ref_count));
			}

			void reset(std::nullptr_t null_p) {
				if(ref_obj != nullptr)
                                {
                                        if(--(*(ref_obj->ref_count)) == 0)
                                        {
                                                delete ref_obj;
                                        }

                                        ref_obj = nullptr;
                                        data = nullptr;
                                }
			}

			SharedPtr& operator=(const SharedPtr& sp) {
				if(this == &sp) {
					return *this;
				}
				if(ref_obj != nullptr && --(*(ref_obj->ref_count)) == 0) {
					delete ref_obj;
					ref_obj = nullptr;
					data = nullptr;
				}
				ref_obj = sp.ref_obj;
				if(ref_obj != nullptr) {
					++(*(ref_obj->ref_count));
				}
				data = sp.get();

				return *this;
			}

			template <typename U>
			SharedPtr& operator=(const SharedPtr<U>& sp) {
				if(this == (SharedPtr<T>*)&sp) {
                                        return *this;
                                }
                                if(ref_obj != nullptr && --(*(ref_obj->ref_count)) == 0) {
                                        delete ref_obj;
                                        ref_obj = nullptr;
                                        data = nullptr;
                                }
                                ref_obj = (RefObject<T>*)sp.ref_obj;
                                if(ref_obj != nullptr) {
                                        ++(*(ref_obj->ref_count));
                                }
                                data = static_cast<T*>(sp.get());

                                return *this;
			}

			T& operator*() const {
				return (*data);
			}

			T* operator->() const {
				return data;
			}

			explicit operator bool() const {
				return (data != nullptr);
			}

	};

	template <typename T1, typename T2>
        bool operator==(const SharedPtr<T1>& lhs, const SharedPtr<T2>& rhs) {
                return (lhs.get() == rhs.get() || (lhs == nullptr && rhs == nullptr));
        };

        template <typename T1>
        bool operator==(const SharedPtr<T1> &lhs, std::nullptr_t rhs) {
                return (!lhs);
        };

        template <typename T1>
        bool operator==(std::nullptr_t lhs, const SharedPtr<T1> &rhs) {
                return (!rhs);
        };

        template <typename T1, typename T2>
        bool operator!=(const SharedPtr<T1> &lhs, const SharedPtr<T2> &rhs) {
                return !(lhs == rhs);
        };

        template <typename T1>
        bool operator!=(const SharedPtr<T1> &lhs, std::nullptr_t rhs){
                return (bool)lhs;
        };

        template <typename T1>
        bool operator!=(std::nullptr_t lhs, const SharedPtr<T1> &rhs) {
		return (bool)rhs;
        };

	template <typename T, typename U>
        SharedPtr<T> static_pointer_cast(const SharedPtr<U> &sp)
        {
                SharedPtr<T> sptr(sp);
                sptr.data = static_cast<T*>(sptr.data);
                return sp;
        };

        template <typename T, typename U>
        SharedPtr<T> dynamic_pointer_cast(const SharedPtr<U> &sp)
        {
                SharedPtr<T> sptr(sp);
                sptr.data = dynamic_cast<T*>(sp.data);
                return sptr;
        };
}

#endif
