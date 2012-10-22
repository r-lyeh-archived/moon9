#if   defined(NDEBUG) || defined(_NDEBUG)
const bool is_debug = false;
const bool is_release = true;
//#elif defined(DEBUG) || defined(_DEBUG)
#else
const bool is_debug = true;
const bool is_release = false;
#endif
