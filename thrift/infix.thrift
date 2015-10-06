/**
 * The first thing to know about are types. The available types in Thrift are:
 *
 *  bool        Boolean, one byte
 *  byte        Signed byte
 *  i16         Signed 16-bit integer
 *  i32         Signed 32-bit integer
 *  i64         Signed 64-bit integer
 *  double      64-bit floating point value
 *  string      String
 *  binary      Blob (byte array)
 *  map<t1,t2>  Map from one type to another
 *  list<t1>    Ordered list of one type
 *  set<t1>     Set of unique elements of one type
 *
 * Did you also notice that Thrift supports C style comments?
 */

/**
 * Structs are the basic complex data structures. They are comprised of fields
 * which each have an integer identifier, a type, a symbolic name, and an
 * optional default value.
 *
 * Fields can be declared "optional", which ensures they will not be included
 * in the serialized output if they aren't set.  Note that this requires some
 * manual management in some languages.
 */

/**
 * Structs can also be exceptions, if they are nasty.
 */

//struct Work {
//  1: i32 num1 = 0,
//  2: i32 num2,
//  3: optional string comment,
//}

//exception InvalidOperation {
//  1: i32 whatOp,
//  2: string why
//}

namespace cpp infix
namespace java infix

/**
 * Ahh, now onto the cool part, defining a service. Services just need a name
 * and can optionally inherit from another service using the extends keyword.
 */

service RemoteInfixSearch{
   i32 ping(),
   list<string> findMatches(1:string key),
   i32 registerService(1:string host,2:i32 port),
   i32 deregisterService(1:string host,2:i32 port),
}

