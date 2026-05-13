Import("env")
import os
import re

def patch_wifi_driver():
    print("🔍 Searching for esp_wifi_api.c to patch...")
    
    # Base path for PlatformIO packages on GitHub runner
    base_path = os.path.expanduser("~/.platformio/packages/framework-arduinoespressif32")
    
    # Possible paths for esp_wifi_api.c in different framework versions
    possible_paths = [
        os.path.join(base_path, "tools/sdk/esp32s3/include/esp_wifi/src/esp_wifi_api.c"),
        os.path.join(base_path, "tools/sdk/esp32s3/esp-wifi/libesp_wifi.a"), # fallback
        os.path.join(base_path, "libraries/ESP32/src/esp32-hal-wifi.c"),
    ]
    
    # Also search recursively in case structure changed
    target_file = None
    for root, dirs, files in os.walk(base_path):
        if "esp_wifi_api.c" in files:
            target_file = os.path.join(root, "esp_wifi_api.c")
            break
    
    if not target_file:
        print("⚠️  esp_wifi_api.c not found in standard paths. Trying alternative patch method...")
        # Try to patch by modifying the compiled library flags instead
        env.Append(CPPDEFINES=["DISABLE_WIFI_MGMT_FILTER=1"])
        print("✅ Added compile flag DISABLE_WIFI_MGMT_FILTER=1")
        return

    print(f"✅ Found: {target_file}")
    
    with open(target_file, "r") as f:
        content = f.read()

    original = content
    
    # Pattern 1: The main deauth filter check
    content = re.sub(
        r'ESP_RETURN_ON_FALSE\(\(frame_ctrl & 0x0c\) == 0x08 \|\| \(frame_ctrl & 0x0c\) == 0x00, ESP_ERR_WIFI_ARG, TAG, "unsupport frame type: 0x%02x", frame_ctrl\);',
        '// PATCHED: Allow all frame types (Deauth enabled)\n// ESP_RETURN_ON_FALSE(...)',
        content
    )
    
    # Pattern 2: Additional management frame filter
    content = re.sub(
        r'ESP_RETURN_ON_FALSE\(frame_type == WIFI_PKT_BEACON \|\| frame_type == WIFI_PKT_PROBE_RESP, ESP_ERR_WIFI_ARG, TAG, "unsupport frame type: 0x%02x", frame_ctrl\);',
        '// PATCHED: Allow all mgmt frames\n// ESP_RETURN_ON_FALSE(...)',
        content
    )
    
    # Pattern 3: Generic "unsupport frame type" check
    if 'unsupport frame type' in content:
        # Comment out any line containing this error message
        lines = content.split('\n')
        new_lines = []
        for line in lines:
            if 'unsupport frame type' in line and 'ESP_RETURN_ON_FALSE' in line:
                new_lines.append('// PATCHED: ' + line)
            else:
                new_lines.append(line)
        content = '\n'.join(new_lines)

    if content != original:
        with open(target_file, "w") as f:
            f.write(content)
        print("🔓 DRIVER PATCHED: Deauth frames should now pass!")
    else:
        print("⚠️  No matching patterns found. The filter might be in a different form.")

patch_wifi_driver()
