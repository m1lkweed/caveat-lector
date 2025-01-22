// Various convenience macros
// (c)m1lkweed, 2024 LGPLv3+

// This header gets used in a lot of programs; this just makes sure the latest version is always used.
#if !defined(CAVEAT_LECTOR_H) || (CAVEAT_LECTOR_H <= 2024101100ULL)
#undef CAVEAT_LECTOR_H
#define CAVEAT_LECTOR_H 2024101100ULL

#undef types_compatible_p
#undef typecmp
#undef force_bool_consteval
#undef bit_cast
#undef aligned_storage
#undef choose_expr
#undef constant_p
#undef coerce_to_type
#undef type_to_object
#undef decay
#undef swap
#undef add_const
#undef add_volatile
#undef add_atomic
#undef add_cv
#undef add_cva
#undef add_ca
#undef add_va
#undef remove_cv
#undef remove_const
#undef remove_volatile
#undef remove_atomic
#undef apply_cva_to_type
#undef is_void
#undef is_bool
#undef is_arr
#undef is_func
#undef is_const
#undef is_volatile
#undef is_atomic
#undef is_unsigned
#undef is_signed
#undef is_bounded_array
#undef is_unbounded_array
#undef is_string_literal
#undef is_empty
#undef is_corresponding_member
#undef is_ellipsis
#undef bool_is_c23_bool

// Caveat lector: black magic lies below this line.

// Compares types of both parameters. Returns true if the types are compatible, otherwise false.
// Parameter y may not be a variably-modified type. Paramters may be types or expressions.
// Designed to be a drop-in replacement for gcc's __builtin_types_compatible_p.
#define types_compatible_p(x, y) _Generic((typeof_unqual(x)*){}, typeof_unqual(y)*:1, default:0)

// Compares qualified types of both parameters. Returns true if the types are compatible and like-qualified,
// otherwise false. Parameters may be types or expressions.
// Parameter y may not be a variably-modified type.
#define typecmp(x, y) _Generic((typeof(x)*){}, typeof(y)*:1, default:0)

// If possible, forces a boolean statement to be evaluated at compile-time.
// This is meant to force less-optimized compilers to preform evaluations that they might otherwise delay until run-time.
#define force_bool_consteval(...) types_compatible_p( \
	(_Generic((int(*)[(__VA_ARGS__) + 1]){}, \
		int(*)[1]: (struct{signed a;}){}, \
		int(*)[2]: (struct{unsigned a;}){} \
	)).a, \
	unsigned \
)

// Cast the bits of arg to type. Parameter type must be a type, not a literal or object with that type.
// Designed to be compatible with g++'s __builtin_bit_cast.
#define bit_cast(type, ...) ( \
	union{typeof(__VA_ARGS__) in; typeof(type) out; \
	int enforce_type:_Generic((int(*)(int(type)))0, \
	int(*)(int):-1, default:1);} \
){__VA_ARGS__}.out

// Returns the address for a block of memory with the specified alignment capable of holding the specified type.
// Has block-scope lifetime. Designed to be a drop-in replacement for c++'s std::aligned_storage.
#define aligned_storage(T, alignment) ((typeof(T)*)((struct{_Alignas((alignment))unsigned char d[sizeof(T)];}){}).d)

// Selects between one of two expressions based on a compile-time constant.
// Return type matches the chosen expression. Designed to be a drop-in replacement for gcc's __builtin_choose_expr.
#define choose_expr(cond, truthy, falsy) _Generic((int(*)[1 + !!(cond)])0, int(*)[2]:(truthy), int(*)[1]:(falsy))

// Returns true if argument is a constant expression, otherwise false.
// This is NOT a drop-in replacement for gcc's __builtin_constant_p as it cannot handle structs.
#define constant_p(...) _Generic((int(*)[1 + !(__VA_ARGS__)])0, int(*)[3]:0, default:1)

// Returns an expression with the type of type. If arg is not the correct type, an empty literal is returned.
// This may be useful for working around the syntactic-validity requirements of unevaluated branches of the _Generic() selector.
#define coerce_to_type(type, ...) _Generic(((typeof(__VA_ARGS__)*){}), typeof(type)*:(__VA_ARGS__), default:(type){})

// Returns an empty literal of the specified type. Useful for cases where raw types are desired but are disallowed by the language.
// Function types and the void type can be used here, which helps with _Generic() magic like the decay() macro below.
#define type_to_object(...) (*((typeof(__VA_ARGS__)*){}))

// Returns an empty literal of the type of the argument after type decay. Arrays and functions are converted to pointers.
// This is treated as NOT being constexpr because of type_to_object().
#define decay(...) (1?type_to_object(__VA_ARGS__):type_to_object(__VA_ARGS__))

// Swaps the values pointed to by its arguments. Works for all types that can be assigned to, i.e. not arrays.
// Arguments are evaluated twice: once for assignment-to, once for assignment-from.
#define swap(x, y) ((void)(*(x) = ((struct{typeof(*(x)) first;typeof(*(y)) second;}){*(y), *(y) = *(x)}.first)))

// Returns the argument with additional type qualifications. Invalid qualifications (such as atomic on an array) are ignored.
#define add_const(...) ((const typeof(__VA_ARGS__))(__VA_ARGS__))
#define add_volatile(...) ((volatile typeof(__VA_ARGS__))(__VA_ARGS__))
#define add_atomic(...) choose_expr(is_arr(__VA_ARGS__), __VA_ARGS__, (_Atomic typeof(choose_expr(is_arr(__VA_ARGS__), 0, __VA_ARGS__)))(choose_expr(is_arr(__VA_ARGS__), 0, __VA_ARGS__)))
#define add_cv(...) ((const volatile typeof(__VA_ARGS__))(__VA_ARGS__))
#define add_cva(...) add_atomic((const volatile typeof(__VA_ARGS__))(__VA_ARGS__))
#define add_ca(...) add_atomic((const typeof(__VA_ARGS__))(__VA_ARGS__))
#define add_va(...) add_atomic((volatile typeof(__VA_ARGS__))(__VA_ARGS__))

// Returns the argument stripped of specified qualifications.
#define remove_cv(...) choose_expr(is_atomic(__VA_ARGS__), ((_Atomic typeof_unqual(choose_expr(is_arr(__VA_ARGS__), 0, (__VA_ARGS__))))(__VA_ARGS__)), ((typeof_unqual(__VA_ARGS__))(__VA_ARGS__)))
#define remove_const(...) choose_expr(is_volatile(__VA_ARGS__), (typeof(add_volatile(typeof(choose_expr(is_atomic(__VA_ARGS__), (add_atomic(typeof_unqual(__VA_ARGS__))){}))))){})
#define remove_volatile(...) choose_expr(is_const(__VA_ARGS__), (typeof(add_const(typeof(choose_expr(is_atomic(__VA_ARGS__), (add_atomic(typeof_unqual(__VA_ARGS__))){}))))){})
#define remove_atomic(...) choose_expr(is_const(__VA_ARGS__), choose_expr(is_volatile(__VA_ARGS__), ((const volatile typeof_unqual(__VA_ARGS__))(__VA_ARGS__)), \
	((const typeof_unqual(__VA_ARGS__))(__VA_ARGS__))), choose_expr(is_volatile(__VA_ARGS__), \((volatile typeof_unqual(__VA_ARGS__))(__VA_ARGS__)), ((typeof_unqual(__VA_ARGS__))(__VA_ARGS__))))

// Returns an object of type T with type-qualifications matching the type of CVA.
// If T has stricter qualifications than CVA, they are stripped; else, they are added.
// Invalid qualifications (such as `atomic` on an array) are ignored.
#define apply_cva_to_type(CVA, T) choose_expr( \
	is_atomic(CVA), \
		choose_expr(is_const(CVA), \
			choose_expr(is_volatile(CVA), \
				(typeof(add_cva(typeof_unqual(T)))){}, \
				(typeof(add_ca(typeof_unqual(T)))){} \
			), \
			choose_expr(is_volatile(CVA), \
				(typeof(add_va(typeof_unqual(T)))){}, \
				(typeof(add_atomic(typeof_unqual(T)))){} \
			) \
		), \
		choose_expr(is_const(CVA), \
			choose_expr(is_volatile(CVA), \
				(typeof(add_cv(typeof_unqual(T)))){}, \
				(typeof(add_const(typeof_unqual(T)))){} \
			), \
		choose_expr(is_volatile(CVA), \
			(typeof(add_volatile(typeof_unqual(T)))){}, \
			(typeof_unqual(T)){} \
		) \
	) \
)

// Returns true if argument's type is the incomplete type `void`, otherwise false.
#define is_void(...) _Generic((typeof(__VA_ARGS__)*){}, void*:1, default:0)

// Returns true if argument's type is `bool`, otherwise false.
#define is_bool(...) _Generic((typeof(__VA_ARGS__)*){}, bool*:1, default:0)

// Returns true if argument's type is is any kind of array, otherwise false. Note that pointers are not arrays.
#define is_arr(...) (!(types_compatible_p(decay(__VA_ARGS__), (__VA_ARGS__)) || is_func(__VA_ARGS__)))

// Returns true if argument's type is is any kind of function, otherwise false. Note that function pointers are not functions.
#define is_func(...) _Generic(*(typeof(__VA_ARGS__)*){}, typeof(__VA_ARGS__)*:1, default:0)

// Returns true if argument's type has a top-level `const` qualifier, otherwise false.
#define is_const(...) choose_expr(is_arr(__VA_ARGS__), typecmp(typeof(typeof(__VA_ARGS__) const volatile), typeof(typeof(__VA_ARGS__) volatile)), typecmp(typeof(typeof(choose_expr(is_arr(__VA_ARGS__), 0, __VA_ARGS__)) const volatile _Atomic), typeof(typeof(choose_expr(is_arr(__VA_ARGS__), 0, __VA_ARGS__)) volatile _Atomic)))

// Returns true if argument's type has a top-level `volatile` qualifier, otherwise false.
#define is_volatile(...) choose_expr(is_arr(__VA_ARGS__), typecmp(typeof(typeof(__VA_ARGS__) const volatile), typeof(typeof(__VA_ARGS__) const)), typecmp(typeof(typeof(choose_expr(is_arr(__VA_ARGS__), 0, __VA_ARGS__)) const volatile _Atomic), typeof(typeof(choose_expr(is_arr(__VA_ARGS__), 0, __VA_ARGS__)) const _Atomic)))

// Returns true if argument's type has a top-level `atomic` qualifier, otherwise false.
#define is_atomic(...) choose_expr(is_arr(__VA_ARGS__), 0, typecmp(typeof(typeof(choose_expr(is_arr(__VA_ARGS__), 0, __VA_ARGS__)) const volatile _Atomic), typeof(typeof(__VA_ARGS__) const volatile)))

// Returns true if argument's type is unsigned. Cannot handle non-scalar types, otherwise false.
#define is_unsigned(...) ((~(typeof(__VA_ARGS__))0 > 0) && !types_compatible_p((__VA_ARGS__), char))

// Returns true if argument's type is signed, otherwise false. Cannot handle non-scalar types.
#define is_signed(...) ((~(typeof(__VA_ARGS__))0 < 0) && !types_compatible_p((__VA_ARGS__), char))

// Returns true if argument is a fixed-length array, otherwise false.
#define is_bounded_array(...) (is_arr(__VA_ARGS__) && constant_p(sizeof(__VA_ARGS__)))

// Returns true if argument is a variable-length array, otherwise false.
#define is_unbounded_array(...) (!is_bounded_array(__VA_ARGS__))

// Returns true if argument is any kind of string literal, including wide, universal-character, or UTF-8; otherwise false.
// will not return true for compound literals.
#define is_string_literal(...) ((#__VA_ARGS__[((sizeof(#__VA_ARGS__)-2) > 0)?sizeof(#__VA_ARGS__)-2:0] == '\"') && (sizeof(#__VA_ARGS__) > 2))

// Returns true if there is no argument, otherwise false.
#define is_empty(...) (!0 ## __VA_OPT__(1))

// Returns true if `pT.p` can be treated the same as `qT.q` when casting, otherwise false.
// Cannot handle bitfields.
#define is_corresponding_member(pT, qT, p, q) (types_compatible_p(typeof(((typeof(pT)*){})->(p)), \
typeof(((typeof(qT)*){})->(q))) && (_Alignof(typeof(((typeof(pT)*){})->(p))) == _Alignof(typeof(((typeof(qT)*){})->(q)))) && \
(((size_t)&((typeof(pT)*){})->(p)) == ((size_t)&((typeof(qT)*){})->(q))))

// Returns true if argument is literally the sequence `...`, otherwise false.
#define is_ellipsis(...) ((sizeof(#__VA_ARGS__) == 4) && ('.' == #__VA_ARGS__[0]) && ('.' == #__VA_ARGS__[1]) && ('.' == #__VA_ARGS__[2]))

// Returns 1 if `bool` is the c23 keyword.
// If `bool` is a typedef then this returns 0.
#define bool_is_c23_bool() (1 == (bool)0.5)

#endif // CAVEAT_LECTOR_H
