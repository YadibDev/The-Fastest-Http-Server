import urllib.request
import urllib.error

# الرابط المستهدف (تأكد من صحة المسار في خادمك)
url = "http://127.0.0.1:8082/out"

# إعداد الطلب وتحديد الطريقة كـ DELETE
# ملاحظة: طلبات DELETE عادة لا ترسل محتوى (Body)، لذا نترك data=None
req = urllib.request.Request(url, method="DELETE")

# إضافة هيدرز لمحاكاة متصفح أو عميل حقيقي
req.add_header("User-Agent", "Webserv-Tester/1.0")
req.add_header("Accept", "*/*")

try:
    print(f"--- Sending DELETE request to: {url} ---")
    with urllib.request.urlopen(req, timeout=10) as response:
        print(f"Status Code: {response.status} {response.reason}")
        
        # قراءة الهيدرز المرسلة من خادمك
        print("Response Headers:")
        for key, value in response.headers.items():
            print(f"  {key}: {value}")
        
        # قراءة محتوى الرد إن وجد (مثلاً رسالة تأكيد الحذف)
        body = response.read().decode('utf-8')
        if body:
            print(f"\nResponse Body:\n{body}")
        else:
            print("\nResponse Body: [Empty]")

except urllib.error.HTTPError as e:
    # معالجة أخطاء HTTP (مثل 404 إذا كان الملف غير موجود أو 403 إذا كان محمياً)
    print(f"HTTP Error Occurred: {e.code} {e.reason}")
    try:
        error_body = e.read().decode('utf-8')
        print(f"Server Error Message: {error_body}")
    except:
        pass

except urllib.error.URLError as e:
    # معالجة أخطاء الاتصال (مثل أن يكون الخادم مغلقاً)
    print(f"Connection Failed: {e.reason}")

except Exception as e:
    print(f"An unexpected error occurred: {e}")