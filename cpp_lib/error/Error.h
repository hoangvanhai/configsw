#ifndef __err_ERROR_CPP_H__
#define __err_ERROR_CPP_H__
#include <string>

#ifdef __cplusplus
extern "C" {
#endif
namespace cppframework{
namespace err{
    typedef struct{
        int code;
        std::string description;
    }ERR;
    extern ERR UNKNOWN;
    extern ERR DONE    ;//{ .code = 0, .description = "" };
    extern ERR PERM    ;//{ .code = 1, .description = "Operation not permitted" };
    extern ERR NOENT   ;//{ .code = 2, .description = "No such file or directory" };
    extern ERR SRCH    ;//{ .code = 3, .description = "No such process" };
    extern ERR INTR    ;//{ .code = 4, .description = "Interrupted system call" };
    extern ERR IO      ;//{ .code = 5, .description = "I/O error" };
    extern ERR NXIO    ;//{ .code = 6, .description = "No such device or address" };
    extern ERR BIG     ;//{ .code = 7, .description = "Argument list too long" };
    extern ERR NOEXEC  ;//{ .code = 8, .description = "Exec format error" };
    extern ERR BADF    ;//{ .code = 9, .description = "Bad file number" };
    extern ERR CHILD   ;//{ .code =10, .description = "No child processes" };
    extern ERR AGAIN   ;//{ .code =11, .description = "Try again" };
    extern ERR NOMEM   ;//{ .code =12, .description = "Out of memory" };
    extern ERR ACCES   ;//{ .code =13, .description = "Permission denied" };
    extern ERR FAULT   ;//{ .code =14, .description = "Bad address" };
    extern ERR NOTBLK  ;//{ .code =15, .description = "Block device required" };
    extern ERR BUSY    ;//{ .code =16, .description = "Device or resource busy" };
    extern ERR EXIST   ;//{ .code =17, .description = "File exists" };
    extern ERR XDEV    ;//{ .code =18, .description = "Cross-device link" };
    extern ERR NODEV   ;//{ .code =19, .description = "No such device" };
    extern ERR NOTDIR  ;//{ .code =20, .description = "Not a directory" };
    extern ERR ISDIR   ;//{ .code =21, .description = "Is a directory" };
    extern ERR INVAL   ;//{ .code =22, .description = "Invalid argument" };
    extern ERR NFILE   ;//{ .code =23, .description = "File table overflow" };
    extern ERR MFILE   ;//{ .code =24, .description = "Too many open files" };
    extern ERR NOTTY   ;//{ .code =25, .description = "Not a typewriter" };
    extern ERR TXTBSY  ;//{ .code =26, .description = "Text file busy" };
    extern ERR FBIG    ;//{ .code =27, .description = "File too large" };
    extern ERR NOSPC   ;//{ .code =28, .description = "No space left on device" };
    extern ERR SPIPE   ;//{ .code =29, .description = "Illegal seek" };
    extern ERR ROFS    ;//{ .code =30, .description = "Read-only file system" };
    extern ERR MLINK   ;//{ .code =31, .description = "Too many links" };
    extern ERR PIPE    ;//{ .code =32, .description = "Broken pipe" };
    extern ERR DOM     ;//{ .code =33, .description = "Math argument out of domain of func" };
    extern ERR RANGE   ;//{ .code =34, .description = "Math result not representable" };
    extern ERR DEADLK    ;//{ .code =35, .description = "Resource deadlock would occur" };
    extern ERR NAMETOOLONG ;//{ .code = 36, .description = "File name too long" };
    extern ERR NOLCK    ;//{ .code =37, .description = "No record locks available" };
    extern ERR NOSYS    ;//{ .code =38, .description = "Function not implemented" };
    extern ERR NOTEMPTY ;//{ .code =39, .description = "Directory not empty" };
    extern ERR LOOP     ;//{ .code =40, .description = "Too many symbolic links encountered" };
    extern ERR NOMSG    ;//{ .code =42, .description = "No message of desired type" };
    extern ERR IDRM     ;//{ .code =43, .description = "Identifier removed" };
    extern ERR CHRNG    ;//{ .code =44, .description = "Channel number out of range" };
    extern ERR L2NSYNC  ;//{ .code =45, .description = "Level 2 not synchronized" };
    extern ERR L3HLT    ;//{ .code =46, .description = "Level 3 halted" };
    extern ERR L3RST    ;//{ .code =47, .description = "Level 3 reset" };
    extern ERR LNRNG    ;//{ .code =48, .description = "Link number out of range" };
    extern ERR UNATCH   ;//{ .code =49, .description = "Protocol driver not attached" };
    extern ERR NOCSI    ;//{ .code =    50, .description = "No CSI structure available" };
    extern ERR L2HLT    ;//{ .code =    51, .description = "Level 2 halted" };
    extern ERR BADE     ;//{ .code =    52, .description = "Invalid exchange" };
    extern ERR BADR     ;//{ .code =    53, .description = "Invalid request descriptor" };
    extern ERR XFULL    ;//{ .code =    54, .description = "Exchange full" };
    extern ERR NOANO    ;//{ .code =    55, .description = "No anode" };
    extern ERR BADRQC   ;//{ .code =    56, .description = "Invalid request code" };
    extern ERR BADSLT   ;//{ .code =    57, .description = "Invalid slot" };
    extern ERR BFONT    ;//{ .code =    59, .description = "Bad font file format" };
    extern ERR NOSTR    ;//{ .code =    60, .description = "Device not a stream" };
    extern ERR NODATA   ;//{ .code =    61, .description = "No data available" };
    extern ERR TIME     ;//{ .code =    62, .description = "Timer expired" };
    extern ERR NOSR     ;//{ .code =    63, .description = "Out of streams resources" };
    extern ERR NONET    ;//{ .code =    64, .description = "Machine is not on the network" };
    extern ERR NOPKG    ;//{ .code =    65, .description = "Package not installed" };
    extern ERR REMOTE   ;//{ .code =    66, .description = "Object is remote" };
    extern ERR NOLINK   ;//{ .code =    67, .description = "Link has been severed" };
    extern ERR ADV      ;//{ .code =    68, .description = "Advertise error" };
    extern ERR SRMNT    ;//{ .code =    69, .description = "Srmount error" };
    extern ERR COMM     ;//{ .code =    70, .description = "Communication error on send" };
    extern ERR PROTO    ;//{ .code =    71, .description = "Protocol error" };
    extern ERR MULTIHOP ;//{ .code =    72, .description = "Multihop attempted" };
    extern ERR DOTDOT   ;//{ .code =    73, .description = "RFS specific error" };
    extern ERR BADMSG   ;//{ .code =    74, .description = "Not a data message" };
    extern ERR OVER_FLOW;//{ .code =    75, .description = "Value too large for defined data type" };
    extern ERR NOTUNIQ  ;//{ .code =    76, .description = "Name not unique on network" };
    extern ERR BADFD    ;//{ .code =    77, .description = "File descriptor in bad state" };
    extern ERR REMCHG   ;//{ .code =    78, .description = "Remote address changed" };
    extern ERR LIBACC   ;//{ .code =    79, .description = "Can not access a needed shared library" };
    extern ERR LIBBAD   ;//{ .code =    80, .description = "Accessing a corrupted shared library" };
    extern ERR LIBSCN   ;//{ .code =    81, .description = ".lib section in a.out corrupted" };
    extern ERR LIBMAX   ;//{ .code =    82, .description = "Attempting to link in too many shared libraries" };
    extern ERR LIBEXEC  ;//{ .code =    83, .description = "Cannot exec a shared library directly" };
    extern ERR ILSEQ    ;//{ .code =    84, .description = "Illegal byte sequence" };
    extern ERR RESTART  ;//{ .code =    85, .description = "Interrupted system call should be restarted" };
    extern ERR STRPIPE  ;//{ .code =    86, .description = "Streams pipe error" };
    extern ERR USERS    ;//{ .code =    87, .description = "Too many users" };
    extern ERR NOTSOCK  ;//{ .code =    88, .description = "Socket operation on non-socket" };
    extern ERR DESTADDRREQ   ;//{ .code =89, .description = "Destination address required" };
    extern ERR MSGSIZE  ;//{ .code =90, .description = "Message too long" };
    extern ERR PROTOTYPE;//{ .code =91, .description = "Protocol wrong type for socket" };
    extern ERR NOPROTOOPT    ;//{ .code =92, .description = "Protocol not available" };
    extern ERR PROTONOSUPPORT;//{ .code =93, .description = "Protocol not supported" };
    extern ERR SOCKTNOSUPPORT;//{ .code =94, .description = "Socket type not supported" };
    extern ERR OPNOTSUPP;//{ .code =95, .description = "Operation not supported on transport endpoint" };
    extern ERR PFNOSUPPORT   ;//{ .code =96, .description = "Protocol family not supported" };
    extern ERR AFNOSUPPORT   ;//{ .code =97, .description = "Address family not supported by protocol" };
    extern ERR ADDRINUSE     ;//{ .code =98, .description = "Address already in use" };
    extern ERR ADDRNOTAVAIL  ;//{ .code =    99, .description = "Cannot assign requested address" };
    extern ERR NETDOWN  ;//{ .code =100, .description = "Network is down" };
    extern ERR NETUNREACH    ;//{ .code =    101, .description = "Network is unreachable" };
    extern ERR NETRESET ;//{ .code =    102, .description = "Network dropped connection because of reset" };
    extern ERR CONNABORTED   ;//{ .code =    103, .description = "Software caused connection abort" };
    extern ERR CONNRESET;//{ .code =    104, .description = "Connection reset by peer" };
    extern ERR NOBUFS        ;//{ .code =105, .description = "No buffer space available" };
    extern ERR ISCONN        ;//{ .code =106, .description = "Transport endpoint is already connected" };
    extern ERR NOTCONN  ;//{ .code =107, .description = "Transport endpoint is not connected" };
    extern ERR SHUTDOWN ;//{ .code =    108, .description = "Cannot send after transport endpoint shutdown" };
    extern ERR TOOMANYREFS   ;//{ .code =    109, .description = "Too many references: cannot splice" };
    extern ERR TIMEDOUT ;//{ .code =    110, .description = "Connection timed out" };
    extern ERR CONNREFUSED   ;//{ .code =    111, .description = "Connection refused" };
    extern ERR HOSTDOWN;//{ .code =    112, .description = "Host is down" };
    extern ERR HOSTUNREACH   ;//{ .code =113, .description = "No route to host" };
    extern ERR ALREADY ;//{ .code =    114, .description = "Operation already in progress" };
    extern ERR INPROGRESS;//{ .code =    115, .description = "Operation now in progress" };
    extern ERR STALE   ;//{ .code =    116, .description = "Stale file handle" };
    extern ERR UCLEAN  ;//{ .code =    117, .description = "Structure needs cleaning" };
    extern ERR NOTNAM  ;//{ .code =    118, .description = "Not a XENIX named type file" };
    extern ERR NAVAIL  ;//{ .code =    119, .description = "No XENIX semaphores available" };
    extern ERR ISNAM   ;//{ .code =    120, .description = "Is a named type file" };
    extern ERR REMOTEIO;//{ .code =    121, .description = "Remote I/O error" };
    extern ERR DQUOT   ;//{ .code =    122, .description = "Quota exceeded" };
    extern ERR NOMEDIUM;//{ .code =    123, .description = "No medium found" };
    extern ERR MEDIUMTYPE    ;//{ .code =    124, .description = "Wrong medium type" };
    extern ERR CANCELED;//{ .code =    125, .description = "Operation Canceled" };
    extern ERR NOKEY   ;//{ .code =    126, .description = "Required key not available" };
    extern ERR KEYEXPIRED    ;//{ .code =    127, .description = "Key has expired" };
    extern ERR KEYREVOKED    ;//{ .code =    128, .description = "Key has been revoked" };
    extern ERR KEYREJECTED   ;//{ .code =    129, .description = "Key was rejected by service" };
    extern ERR OWNERDEAD     ;//{ .code =    130, .description = "Owner died" };
    extern ERR NOTRECOVERABLE;//{ .code =    131, .description = "State not recoverable" };
    extern ERR RFKILL  ;//{ .code =    132, .description = "Operation not possible due to RF-kill" };
    extern ERR HWPOISON;//{ .code =    133, .description = "Memory page has hardware error" };

}}
#ifdef __cplusplus
}
#endif
#endif
