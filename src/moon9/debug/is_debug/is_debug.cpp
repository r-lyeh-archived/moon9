#if   defined(NDEBUG) || defined(_NDEBUG)
const bool is_debug = false;
//#elif defined(DEBUG) || defined(_DEBUG)
#else
const bool is_debug = true;
#endif
