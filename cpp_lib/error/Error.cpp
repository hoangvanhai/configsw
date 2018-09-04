
#include "Error.h"

cppframework::err::ERR UNKNOWN {  255,"Unknown" };
cppframework::err::ERR DONE    {  0,  "" };
cppframework::err::ERR PERM    {  1,  "Operation not permitted" };
cppframework::err::ERR NOENT   {  2,  "No such file or directory" };
cppframework::err::ERR SRCH    {  3,  "No such process" };
cppframework::err::ERR INTR    {  4,  "Interrupted system call" };
cppframework::err::ERR IO      {  5,  "I/O error" };
cppframework::err::ERR NXIO    {  6,  "No such device or address" };
cppframework::err::ERR BIG     {  7,  "Argument list too long" };
cppframework::err::ERR NOEXEC  {  8,  "Exec format error" };
cppframework::err::ERR BADF    {  9,  "Bad file number" };
cppframework::err::ERR CHILD   { 10,  "No child processes" };
cppframework::err::ERR AGAIN   { 11,  "Try again" };
cppframework::err::ERR NOMEM   { 12,  "Out of memory" };
cppframework::err::ERR ACCES   { 13,  "Permission denied" };
cppframework::err::ERR FAULT   { 14,  "Bad address" };
cppframework::err::ERR NOTBLK  { 15,  "Block device required" };
cppframework::err::ERR BUSY    { 16,  "Device or resource busy" };
cppframework::err::ERR EXIST   { 17,  "File exists" };
cppframework::err::ERR XDEV    { 18,  "Cross-device link" };
cppframework::err::ERR NODEV   { 19,  "No such device" };
cppframework::err::ERR NOTDIR  { 20,  "Not a directory" };
cppframework::err::ERR ISDIR   { 21,  "Is a directory" };
cppframework::err::ERR INVAL   { 22,  "Invalid argument" };
cppframework::err::ERR NFILE   { 23,  "File table overflow" };
cppframework::err::ERR MFILE   { 24,  "Too many open files" };
cppframework::err::ERR NOTTY   { 25,  "Not a typewriter" };
cppframework::err::ERR TXTBSY  { 26,  "Text file busy" };
cppframework::err::ERR FBIG    { 27,  "File too large" };
cppframework::err::ERR NOSPC   { 28,  "No space left on device" };
cppframework::err::ERR SPIPE   { 29,  "Illegal seek" };
cppframework::err::ERR ROFS    { 30,  "Read-only file system" };
cppframework::err::ERR MLINK   { 31,  "Too many links" };
cppframework::err::ERR PIPE    { 32,  "Broken pipe" };
cppframework::err::ERR DOM     { 33,  "Math argument out of domain of func" };
cppframework::err::ERR RANGE   { 34,  "Math result not representable" };
cppframework::err::ERR DEADLK    { 35,  "Resource deadlock would occur" };
cppframework::err::ERR NAMETOOLONG {  36,  "File name too long" };
cppframework::err::ERR NOLCK    { 37,  "No record locks available" };
cppframework::err::ERR NOSYS    { 38,  "Function not implemented" };
cppframework::err::ERR NOTEMPTY { 39,  "Directory not empty" };
cppframework::err::ERR LOOP     { 40,  "Too many symbolic links encountered" };
cppframework::err::ERR NOMSG    { 42,  "No message of desired type" };
cppframework::err::ERR IDRM     { 43,  "Identifier removed" };
cppframework::err::ERR CHRNG    { 44,  "Channel number out of range" };
cppframework::err::ERR L2NSYNC  { 45,  "Level 2 not synchronized" };
cppframework::err::ERR L3HLT    { 46,  "Level 3 halted" };
cppframework::err::ERR L3RST    { 47,  "Level 3 reset" };
cppframework::err::ERR LNRNG    { 48,  "Link number out of range" };
cppframework::err::ERR UNATCH   { 49,  "Protocol driver not attached" };
cppframework::err::ERR NOCSI    {     50,  "No CSI structure available" };
cppframework::err::ERR L2HLT    {     51,  "Level 2 halted" };
cppframework::err::ERR BADE     {     52,  "Invalid exchange" };
cppframework::err::ERR BADR     {     53,  "Invalid request descriptor" };
cppframework::err::ERR XFULL    {     54,  "Exchange full" };
cppframework::err::ERR NOANO    {     55,  "No anode" };
cppframework::err::ERR BADRQC   {     56,  "Invalid request code" };
cppframework::err::ERR BADSLT   {     57,  "Invalid slot" };
cppframework::err::ERR BFONT    {     59,  "Bad font file format" };
cppframework::err::ERR NOSTR    {     60,  "Device not a stream" };
cppframework::err::ERR NODATA   {     61,  "No data available" };
cppframework::err::ERR TIME     {     62,  "Timer expired" };
cppframework::err::ERR NOSR     {     63,  "Out of streams resources" };
cppframework::err::ERR NONET    {     64,  "Machine is not on the network" };
cppframework::err::ERR NOPKG    {     65,  "Package not installed" };
cppframework::err::ERR REMOTE   {     66,  "Object is remote" };
cppframework::err::ERR NOLINK   {     67,  "Link has been severed" };
cppframework::err::ERR ADV      {     68,  "Advertise error" };
cppframework::err::ERR SRMNT    {     69,  "Srmount error" };
cppframework::err::ERR COMM     {     70,  "Communication error on send" };
cppframework::err::ERR PROTO    {     71,  "Protocol error" };
cppframework::err::ERR MULTIHOP {     72,  "Multihop attempted" };
cppframework::err::ERR DOTDOT   {     73,  "RFS specific error" };
cppframework::err::ERR BADMSG   {     74,  "Not a data message" };
cppframework::err::ERR OVER_FLOW{     75,  "Value too large for defined data type" };
cppframework::err::ERR NOTUNIQ  {     76,  "Name not unique on network" };
cppframework::err::ERR BADFD    {     77,  "File descriptor in bad state" };
cppframework::err::ERR REMCHG   {     78,  "Remote address changed" };
cppframework::err::ERR LIBACC   {     79,  "Can not access a needed shared library" };
cppframework::err::ERR LIBBAD   {     80,  "Accessing a corrupted shared library" };
cppframework::err::ERR LIBSCN   {     81,  ".lib section in a.out corrupted" };
cppframework::err::ERR LIBMAX   {     82,  "Attempting to link in too many shared libraries" };
cppframework::err::ERR LIBEXEC  {     83,  "Cannot exec a shared library directly" };
cppframework::err::ERR ILSEQ    {     84,  "Illegal byte sequence" };
cppframework::err::ERR RESTART  {     85,  "Interrupted system call should be restarted" };
cppframework::err::ERR STRPIPE  {     86,  "Streams pipe error" };
cppframework::err::ERR USERS    {     87,  "Too many users" };
cppframework::err::ERR NOTSOCK  {     88,  "Socket operation on non-socket" };
cppframework::err::ERR DESTADDRREQ   { 89,  "Destination address required" };
cppframework::err::ERR MSGSIZE  { 90,  "Message too long" };
cppframework::err::ERR PROTOTYPE{ 91,  "Protocol wrong type for socket" };
cppframework::err::ERR NOPROTOOPT    { 92,  "Protocol not available" };
cppframework::err::ERR PROTONOSUPPORT{ 93,  "Protocol not supported" };
cppframework::err::ERR SOCKTNOSUPPORT{ 94,  "Socket type not supported" };
cppframework::err::ERR OPNOTSUPP{ 95,  "Operation not supported on transport endpoint" };
cppframework::err::ERR PFNOSUPPORT   { 96,  "Protocol family not supported" };
cppframework::err::ERR AFNOSUPPORT   { 97,  "Address family not supported by protocol" };
cppframework::err::ERR ADDRINUSE     { 98,  "Address already in use" };
cppframework::err::ERR ADDRNOTAVAIL  {     99,  "Cannot assign requested address" };
cppframework::err::ERR NETDOWN  { 100,  "Network is down" };
cppframework::err::ERR NETUNREACH    {     101,  "Network is unreachable" };
cppframework::err::ERR NETRESET {     102,  "Network dropped connection because of reset" };
cppframework::err::ERR CONNABORTED   {     103,  "Software caused connection abort" };
cppframework::err::ERR CONNRESET{     104,  "Connection reset by peer" };
cppframework::err::ERR NOBUFS        { 105,  "No buffer space available" };
cppframework::err::ERR ISCONN        { 106,  "Transport endpoint is already connected" };
cppframework::err::ERR NOTCONN  { 107,  "Transport endpoint is not connected" };
cppframework::err::ERR SHUTDOWN {     108,  "Cannot send after transport endpoint shutdown" };
cppframework::err::ERR TOOMANYREFS   {     109,  "Too many references: cannot splice" };
cppframework::err::ERR TIMEDOUT {     110,  "Connection timed out" };
cppframework::err::ERR CONNREFUSED   {     111,  "Connection refused" };
cppframework::err::ERR HOSTDOWN{     112,  "Host is down" };
cppframework::err::ERR HOSTUNREACH   { 113,  "No route to host" };
cppframework::err::ERR ALREADY {     114,  "Operation already in progress" };
cppframework::err::ERR INPROGRESS{     115,  "Operation now in progress" };
cppframework::err::ERR STALE   {     116,  "Stale file handle" };
cppframework::err::ERR UCLEAN  {     117,  "Structure needs cleaning" };
cppframework::err::ERR NOTNAM  {     118,  "Not a XENIX named type file" };
cppframework::err::ERR NAVAIL  {     119,  "No XENIX semaphores available" };
cppframework::err::ERR ISNAM   {     120,  "Is a named type file" };
cppframework::err::ERR REMOTEIO{     121,  "Remote I/O error" };
cppframework::err::ERR DQUOT   {     122,  "Quota exceeded" };
cppframework::err::ERR NOMEDIUM{     123,  "No medium found" };
cppframework::err::ERR MEDIUMTYPE    {     124,  "Wrong medium type" };
cppframework::err::ERR CANCELED{     125,  "Operation Canceled" };
cppframework::err::ERR NOKEY   {     126,  "Required key not available" };
cppframework::err::ERR KEYEXPIRED    {     127,  "Key has expired" };
cppframework::err::ERR KEYREVOKED    {     128,  "Key has been revoked" };
cppframework::err::ERR KEYREJECTED   {     129,  "Key was rejected by service" };
cppframework::err::ERR OWNERDEAD     {     130,  "Owner died" };
cppframework::err::ERR NOTRECOVERABLE{     131,  "State not recoverable" };
cppframework::err::ERR RFKILL  {     132,  "Operation not possible due to RF-kill" };
cppframework::err::ERR HWPOISON{     133,  "Memory page has hardware error" };

