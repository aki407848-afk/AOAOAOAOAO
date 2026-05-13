Import("env")
import os
import re

def patch_wifi_driver():
    print("🔍 Ищем файл esp_wifi_api.c для патча...")
    
    # Путь к пакетам PlatformIO на сервере GitHub
    base_path = os.path.expanduser("~/.platformio/packages/framework-arduinoespressif32")
    
    # Ищем файл драйвера
    target_file = None
    for root, dirs, files in os.walk(base_path):
        for file in files:
            if file == "esp_wifi_api.c":
                # Нам нужен файл именно из компонентов esp_wifi
                if "esp_wifi" in root:
                    target_file = os.path.join(root, file)
                    break
    
    if not target_file:
        print("❌ Файл esp_wifi_api.c не найден!")
        return

    print(f"✅ Найден: {target_file}")
    
    # Читаем файл
    with open(target_file, "r") as f:
        content = f.read()

    # Патч: Ищем строку с ошибкой "unsupport frame type" и комментируем проверку
    # Мы заменяем проверку на True, чтобы драйвер всегда пропускал кадр
    old_line = 'ESP_RETURN_ON_FALSE((frame_ctrl & 0x0c) == 0x08 || (frame_ctrl & 0x0c) == 0x00, ESP_ERR_WIFI_ARG, TAG, "unsupport frame type: 0x%02x", frame_ctrl);'
    new_line = '// PATCHED BY GITHUB ACTION: ESP_RETURN_ON_FALSE((frame_ctrl & 0x0c) == 0x08 || (frame_ctrl & 0x0c) == 0x00, ESP_ERR_WIFI_ARG, TAG, "unsupport frame type: 0x%02x", frame_ctrl);'
    
    # Более надежный поиск по части строки ошибки
    if 'unsupport frame type' in content:
        # Заменяем логику проверки на разрешение всего
        content = content.replace(
            'ESP_RETURN_ON_FALSE((frame_ctrl & 0x0c) == 0x08 || (frame_ctrl & 0x0c) == 0x00, ESP_ERR_WIFI_ARG, TAG, "unsupport frame type: 0x%02x", frame_ctrl);',
            '// PATCHED: Allow all frame types\n'
        )
        
        # Также убираем проверку на Management frames, если она есть отдельно
        content = content.replace(
            'ESP_RETURN_ON_FALSE(frame_type == WIFI_PKT_BEACON || frame_type == WIFI_PKT_PROBE_RESP, ESP_ERR_WIFI_ARG, TAG, "unsupport frame type: 0x%02x", frame_ctrl);',
            '// PATCHED: Allow all mgmt frames\n'
        )

        # Записываем обратно
        with open(target_file, "w") as f:
            f.write(content)
        print("🔓 ДРАЙВЕР ВЗЛОМАН: Деаут разрешен!")
    else:
        print("⚠️ Строка блокировки не найдена (возможно, версия ядра другая).")

patch_wifi_driver()
