# Caveat Lector: Reader Beware
Various convenience macros for C that I haven't seen elsewhere.
### `types_compatible_p(x, y)`
---
Compares types of both parameters. Returns true if the types are compatible, otherwise false.
Parameter y may not be a variably-modified type. Paramters may be types or expressions.
Designed to be a drop-in replacement for gcc's `__builtin_types_compatible_p()`.

### `typecmp(x, y)`
---
Compares qualified types of both parameters. Returns true if the types are compatible and like-qualified,
otherwise false. Parameters may be types or expressions.
Parameter y may not be a variably-modified type.

### `force_bool_consteval(...)`
---
If possible, forces a boolean statement to be evaluated at compile-time.
This is meant to force less-optimized compilers to preform evaluations that they might otherwise delay until run-time.

### `bit_cast(type, ...)`
---
Cast the bits of arg to type. Parameter type must be a type, not a literal or object with that type.
Designed to be compatible with g++'s `__builtin_bit_cast()`.

### `aligned_storage(T, alignment)`
---
Returns the address for a block of memory with the specified alignment capable of holding the specified type.
Has block-scope lifetime. Designed to be a drop-in replacement for c++'s std::aligned_storage.

### `choose_expr(cond, truthy, falsy)`
---
Selects between one of two expressions based on a compile-time constant.
Return type matches the chosen expression. Designed to be a drop-in replacement for gcc's __builtin_choose_expr.

### `constant_p(...)`
---
Returns true if argument is a constant expression, otherwise false.
This is NOT a drop-in replacement for gcc's `__builtin_constant_p()` as it cannot handle structs.

### `coerce_to_type(type, ...)`
---
Returns an expression with the type of type. If arg is not the correct type, an empty literal is returned.
This may be useful for working around the syntactic-validity requirements of unevaluated branches of the _Generic() selector.

### `type_to_object(...)`
---
Returns an empty literal of the specified type. Useful for cases where raw types are desired but are disallowed by the language.
Function types and the void type can be used here, which helps with _Generic() magic like the decay() macro below.

### `decay(...)`
---
Returns an empty literal of the type of the argument after type decay. Arrays and functions are converted to pointers.
This is treated as NOT being constexpr because of type_to_object().

### `swap(x, y)`
---
Swaps the values pointed to by its arguments. Works for all types that can be assigned to, i.e. not arrays.
Arguments are evaluated twice: once for assignment-to, once for assignment-from.

### `add_const(...)`
### `add_volatile(...)`
### `add_atomic(...)`
### `add_cv(...)`
### `add_cva(...)`
### `add_ca(...)`
### `add_va(...)`
---
Returns the argument with additional type qualifications. Invalid qualifications (such as atomic on an array) are ignored.

### `remove_cv(...)`
### `remove_const(...)`
### `remove_volatile(...)`
### `remove_atomic(...)`
---
Returns the argument stripped of specified qualifications.

### `apply_cva_to_type(CVA, T)`
---
Returns an object of type T with type-qualifications matching the type of CVA.
If T has stricter qualifications than CVA, they are stripped; else, they are added.
Invalid qualifications (such as `atomic` on an array) are ignored.

### `is_void(...)`
---
Returns true if argument's type is the incomplete type `void`, otherwise false.

### `is_bool(...)`
---
Returns true if argument's type is `bool`, otherwise false.

### `is_arr(...)`
---
Returns true if argument's type is is any kind of array, otherwise false. Note that pointers are not arrays.

### `is_func(...)`
---
Returns true if argument's type is is any kind of function, otherwise false. Note that function pointers are not functions.

### `is_const(...)`
---
Returns true if argument's type has a top-level `const` qualifier, otherwise false.

### `is_volatile(...)`
---
Returns true if argument's type has a top-level `volatile` qualifier, otherwise false.

### `is_atomic(...)`
---
Returns true if argument's type has a top-level `atomic` qualifier, otherwise false.

### `is_unsigned(...)`
---
Returns true if argument's type is unsigned. Cannot handle non-scalar types, otherwise false.

### `is_signed(...)`
---
Returns true if argument's type is signed, otherwise false. Cannot handle non-scalar types.

### `is_bounded_array(...)`
---
Returns true if argument is a fixed-length array, otherwise false.

### `is_unbounded_array(...)`
---
Returns true if argument is a variable-length array, otherwise false.

### `is_string_literal(...)`
---
Returns true if argument is any kind of string literal, including wide, universal-character, or UTF-8; otherwise false.
will not return true for compound literals.

### `is_empty(...)`
---
Returns true if there is no argument, otherwise false.

### `is_corresponding_member(pT, qT, p, q)`
---
Returns true if `pT.p` can be treated the same as `qT.q` when casting, otherwise false.
Cannot handle bitfields.

### `is_ellipsis(...)`
---
Returns true if argument is literally the sequence `...`, otherwise false.

### `bool_is_c23_bool()`
---
Returns 1 if `bool` is the c23 keyword.
If `bool` is a typedef then this returns 0.
