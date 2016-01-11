#pragma once

#include <iostream>
#include <map>
#include <set>
#include <functional>


namespace Skident
{
    using Type = std::map<void*, int>; // first -> T* shared memory, second -> holders

    template<class T/*, class Allocator = std::allocator*/>
	class bkc_shared_ptr
	{
	private: 
		T* m_ptr = nullptr;

        std::function<void(void)> m_deleter;

		static Type shared_memory;

        void inc_holders()
        {
            if (m_ptr == nullptr)
                return;

            shared_memory[m_ptr]++;
        }

        void dec_holders()
        {
            if (m_ptr == nullptr)
                return;

            auto it = shared_memory.find(m_ptr);
            if (it != shared_memory.end())
            {
                if (it->second > 0)
                    it->second--;
            }
        }



	public:
        explicit bkc_shared_ptr(T* ptr = nullptr)
		{
			m_ptr = ptr;
            inc_holders();

            m_deleter = [=](){delete ptr;};
		}

        template<class Deleter>
        bkc_shared_ptr(T* ptr, Deleter del)
            : bkc_shared_ptr(ptr)
        {
            m_deleter = [=](){del(ptr);};
        }

		~bkc_shared_ptr()
		{
            dec_holders();
            if (use_count() == 0)
                m_deleter();
		}

        bkc_shared_ptr& operator=(const bkc_shared_ptr& rhs) noexcept
        {
            dec_holders();
            m_ptr = rhs.m_ptr;
            inc_holders();
            return *this;
        }

		bkc_shared_ptr(const bkc_shared_ptr& rhs)
		{
            m_ptr = rhs.m_ptr;
            inc_holders();
		}

		T* get() const
		{
			return m_ptr;
		}

		T* operator->()
		{
			return m_ptr;
		}

        T& operator*()
        {
            return *m_ptr;
        }

        int use_count() const
        {
            auto it = shared_memory.find(m_ptr);
            if (it != shared_memory.end())
                return it->second;
            return 0;
        }

        void swap(bkc_shared_ptr& rhs) noexcept
        {
            std::swap(m_ptr, rhs.m_ptr);
        }

        void reset() noexcept
        {
            dec_holders();
        }
		


        template <class U>
        void reset (U* p)
        {
            dec_holders();
            m_ptr = p;
            inc_holders();
        }

        // TODO:
        template <class U, class D> void reset (U* p, D del)
        {

        }

        template <class U, class D, class Alloc> void reset (U* p, D del, Alloc alloc)
        {

        }

        bool unique() const noexcept
        {
            return (use_count() == 1);
        }

        /**
         * @brief operator bool - Check if not null
         * @returns whether the stored pointer is a null pointer.
         */
        explicit operator bool() const noexcept
        {
            return (get() != nullptr);
        }
	};

    template<class T>
    Type bkc_shared_ptr<T>::shared_memory;



}

