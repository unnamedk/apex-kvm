#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <numeric>
#include <cmath>
#include <utility>

namespace apex::math
{
    template <class DerivedClass, typename VectorType, size_t VectorSize>
    class base_array
    {
    public:
        using array_type = std::array<VectorType, VectorSize>;

        // Constructors
        constexpr base_array() { m_data.fill( VectorType {} ); }
        constexpr base_array( const base_array &rhs ) { m_data = rhs.m_data; }

        // Arithmetic operators
        template <typename T>
        constexpr DerivedClass operator+( const T &rhs ) const
        {
            return apply( rhs, std::plus<VectorType> {} );
        }

        template <typename T>
        constexpr DerivedClass operator-( const T &rhs ) const
        {
            return apply( rhs, std::minus<VectorType> {} );
        }

        template <typename T>
        constexpr DerivedClass operator*( const T &rhs ) const
        {
            return apply( rhs, std::multiplies<VectorType> {} );
        }

        template <typename T>
        constexpr DerivedClass operator/( const T &rhs ) const
        {
            return apply( rhs, std::divides<VectorType> {} );
        }

        template <typename T>
        constexpr DerivedClass &operator=( const T &rhs )
        {
            if constexpr ( std::is_class_v<T> ) {
                m_data = rhs.m_data;
            } else {
                m_data.fill( rhs );
            }

            return *this;
        }

        // Helper functions
        constexpr VectorType length_sqr() const
        { // squared length of a vector, e.g. (x * x) + (y * y), etc...
            return std::accumulate( cbegin(), cend(), VectorType {}, []( VectorType a, VectorType b ) {
                return a + ( b * b );
            } );
        }
        constexpr VectorType length() const { return std::sqrt( length_sqr() ); }
        constexpr VectorType distance_from( const DerivedClass &rhs ) const { return ( *this - rhs ).length(); }
        constexpr bool is_zero() const
        {
            return std::all_of( cbegin(), cend(), []( VectorType x ) {
                return x == VectorType {};
            } );
        }
        constexpr bool is_valid() const
        {
            return std::all_of( cbegin(), cend(), std::isfinite<VectorType> );
        }

        // Element access operators
        constexpr VectorType &at( size_t i ) { return m_data[ i ]; }
        constexpr const VectorType &at( size_t i ) const { return m_data[ i ]; }

        constexpr VectorType &operator[]( size_t i ) { return m_data[ i ]; }
        constexpr const VectorType &operator[]( size_t i ) const { return m_data[ i ]; }

        constexpr auto data() { return m_data.data(); }
        constexpr const auto data() const { return m_data.data(); }

        constexpr auto begin() { return m_data.begin(); }
        constexpr auto cbegin() const { return m_data.cbegin(); }

        constexpr auto end() { return m_data.end(); }
        constexpr auto cend() const { return m_data.cend(); }

    private:
        template <class Type, typename BinaryOp>
        constexpr DerivedClass apply( const Type &rhs, BinaryOp op ) const
        {
            DerivedClass ans;

            // class
            if constexpr ( std::is_class_v<Type> ) {
                std::transform( m_data.cbegin(), m_data.cend(), rhs.cbegin(), ans.begin(), op );
            }

            // literal
            else {
                std::transform( m_data.cbegin(), m_data.cend(), ans.m_data.begin(), [ op, &rhs ]( const auto &val ) {
                    return op( val, rhs );
                } );
            }

            return DerivedClass( ans );
        }

        std::array<VectorType, VectorSize> m_data;
    };
}