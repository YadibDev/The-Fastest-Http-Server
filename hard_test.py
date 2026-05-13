# generate_hard_html.py
with open("hard_test.html", "w") as f:
    f.write("<!DOCTYPE html>\n<html>\n<body>\n")
    
    # 1. تداخل عميق جداً (Deep Nesting) لتجربة الـ Recursion والـ Stack
    for i in range(1000):
        f.write(f'<div id="depth_{i}" class="level_{i}">\n')
    
    f.write("<h1>Webserv Stress Test - Deep Nesting</h1>\n")
    
    # 2. سمات (Attributes) ضخمة جداً لتجربة الـ Buffer size في الـ Parsing
    huge_attr = "X" * 1024 * 100  # 100KB attribute
    f.write(f'<p data-huge="{huge_attr}">This paragraph has a 100KB attribute.</p>\n')
    
    # 3. تعليقات ضخمة ومشتتة
    f.write(f"<!-- {'LongComment' * 5000} -->\n")
    
    # 4. عدد هائل من الروابط والبيانات المكررة
    for i in range(50000):
        f.write(f'<a href="/test?query={i}">Link_{i}</a> ')
        if i % 10 == 0: f.write("<br>\n")

    # إغلاق التداخل
    for i in range(1000):
        f.write("</div>\n")
        
    f.write("</body>\n</html>")

print("تم توليد الملف: hard_test.html")