import psutil
import os
import threading
import time

def monitor_usage(interval=1):
    pid = os.getpid()
    process = psutil.Process(pid)
    
    print(f"--- بدء مراقبة العملية (PID: {pid}) ---")
    try:
        while True:
            # استخدام interval داخل cpu_percent ضروري جداً للحصول على قراءة دقيقة
            cpu_usage = process.cpu_percent(interval=interval)
            
            # حساب الذاكرة بالميجابايت
            memory_mb = process.memory_info().rss / (1024 * 1024)
            
            print(f"\r[المراقبة] CPU: {cpu_usage:>5}% | RAM: {memory_mb:>7.2f} MB", end="")
    except KeyboardInterrupt:
        print("\nتم إيقاف المراقبة.")

def simulate_work():
    """وظيفة تقوم بعمليات حسابية بسيطة لرفع استهلاك المعالج مؤقتاً"""
    print("سيتم رفع استهلاك المعالج تلقائياً للتجربة بعد 3 ثوانٍ...")
    time.sleep(3)
    while True:
        # عملية حسابية عشوائية لاستهلاك المعالج
        _ = [x**2 for x in range(5000)]
        time.sleep(0.01)

if __name__ == "__main__":
    # تشغيل "العمل الوهمي" في خلفية البرنامج (Thread)
    work_thread = threading.Thread(target=simulate_work, daemon=True)
    work_thread.start()

    # تشغيل المراقب في الخيط الرئيسي
    monitor_usage()