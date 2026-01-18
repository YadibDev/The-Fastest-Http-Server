// /* =======================================================
//    Class: Request
//    Description: Encapsulates parsed HTTP request data with 
//                 read-only accessors.
//    ======================================================= */

// DEFINE CLASS Request:

//     // ==========================================
//     // 1. الخصائص الخاصة (Private Members)
//     // ==========================================
//     PRIVATE PROPERTY Method _method
//     PRIVATE PROPERTY String _path
//     PRIVATE PROPERTY String _query
//     PRIVATE PROPERTY String _version
//     PRIVATE PROPERTY Map<String, String> _headers
//     PRIVATE PROPERTY List<Byte> _body  // (std::vector<char>)

//     // ==========================================
//     // 2. دوال الوصول العامة (Public Getters)
//     // ==========================================
    
//     // إرجاع طريقة الطلب
//     PUBLIC FUNCTION getMethod() RETURNS Method:
//         RETURN _method
//     END FUNCTION

//     // إرجاع المسار
//     PUBLIC FUNCTION getPath() RETURNS String:
//         RETURN _path
//     END FUNCTION

//     // إرجاع سلسلة الاستعلام
//     PUBLIC FUNCTION getQuery() RETURNS String:
//         RETURN _query
//     END FUNCTION

//     // إرجاع نسخة البروتوكول
//     PUBLIC FUNCTION getVersion() RETURNS String:
//         RETURN _version
//     END FUNCTION

//     // إرجاع جسم الطلب (للقراءة فقط)
//     PUBLIC FUNCTION getBody() RETURNS List<Byte>:
//         RETURN _body
//     END FUNCTION

//     // ==========================================
//     // 3. دوال مساعدة (Helper Functions)
//     // ==========================================

//     /* دالة: getHeader
//        الوصف: بحث آمن عن قيمة داخل الـ Map
//        المدخلات: اسم الهيدر (key)
//        المخرجات: القيمة أو نص فارغ إذا لم يوجد */
//     PUBLIC FUNCTION getHeader(String key) RETURNS String:
        
//         // التحقق من وجود المفتاح في الخريطة
//         IF _headers CONTAINS key THEN:
//             RETURN _headers[key]
//         ELSE:
//             RETURN ""  // (Empty String) غير موجود
//         END IF

//     END FUNCTION

// END CLASS

#include "Client.hpp"
Client::Client(const sockaddr_in &addr){};
