// Test ISO 8601 duration parser
// This is a standalone test that simulates the Arduino String class behavior

#include <iostream>
#include <string>
#include <cstring>
#include <cctype>

// Simulate Arduino String class for testing
class String {
private:
    std::string data;
    
public:
    String(const char* str = "") : data(str) {}
    String(const std::string& str) : data(str) {}
    String(int num) : data(std::to_string(num)) {}
    String(float num) : data(std::to_string(num)) {}
    
    size_t length() const { return data.length(); }
    char charAt(size_t index) const { 
        if (index < data.length()) return data[index];
        return '\0';
    }
    
    void trim() {
        size_t start = data.find_first_not_of(" \t\r\n");
        size_t end = data.find_last_not_of(" \t\r\n");
        if (start == std::string::npos) {
            data = "";
        } else {
            data = data.substr(start, end - start + 1);
        }
    }
    
    void toUpperCase() {
        for (char& c : data) {
            c = std::toupper(c);
        }
    }
    
    String substring(size_t start, size_t end) const {
        if (start >= data.length()) return String("");
        if (end > data.length()) end = data.length();
        return String(data.substr(start, end - start));
    }
    
    String substring(size_t start) const {
        if (start >= data.length()) return String("");
        return String(data.substr(start));
    }
    
    int toInt() const {
        try {
            return std::stoi(data);
        } catch (...) {
            return 0;
        }
    }
    
    const char* c_str() const { return data.c_str(); }
    
    String operator+(const String& other) const {
        return String(data + other.data);
    }
    
    String operator+(const char* other) const {
        return String(data + other);
    }
};

// Mock log function
void log(String msg) {
    std::cout << "[LOG] " << msg.c_str() << std::endl;
}

// Copy the parseISO8601Duration function here
int parseISO8601Duration(String duration) {
  duration.trim();
  duration.toUpperCase(); // Normalize to uppercase
  
  if (duration.length() < 2 || duration.charAt(0) != 'P') {
    return 0; // Must start with 'P'
  }
  
  int totalSeconds = 0;
  int i = 1; // Start after 'P'
  bool inTimePart = false; // Track if we've seen 'T'
  
  while (i < duration.length()) {
    // Check for time designator
    if (duration.charAt(i) == 'T') {
      inTimePart = true;
      i++;
      continue;
    }
    
    // Parse number
    int numStart = i;
    while (i < duration.length() && isdigit(duration.charAt(i))) {
      i++;
    }
    
    if (i == numStart || i >= duration.length()) {
      // No number found or reached end without designator
      return 0;
    }
    
    String numStr = duration.substring(numStart, i);
    int value = numStr.toInt();
    
    if (value <= 0) {
      return 0; // Invalid or zero value
    }
    
    // Get designator character
    char designator = duration.charAt(i);
    i++;
    
    // Process based on designator
    switch (designator) {
      case 'D': // Days (only valid before T)
        if (inTimePart) {
          log("ISO8601: 'D' designator not allowed after 'T'");
          return 0;
        }
        totalSeconds += value * 86400; // 24 * 60 * 60
        break;
        
      case 'H': // Hours (only valid after T)
        if (!inTimePart) {
          log("ISO8601: 'H' designator requires 'T' prefix");
          return 0;
        }
        totalSeconds += value * 3600; // 60 * 60
        break;
        
      case 'M': // Minutes (only valid after T)
        if (!inTimePart) {
          log("ISO8601: 'M' designator requires 'T' prefix for minutes");
          return 0;
        }
        totalSeconds += value * 60;
        break;
        
      case 'S': // Seconds (only valid after T)
        if (!inTimePart) {
          log("ISO8601: 'S' designator requires 'T' prefix");
          return 0;
        }
        totalSeconds += value;
        break;
        
      default:
        {
          String msg = String("ISO8601: Unknown designator '");
          msg = msg + String(designator) + String("'");
          log(msg);
          return 0;
        }
    }
  }
  
  return totalSeconds;
}

// Test cases
struct TestCase {
    const char* input;
    int expected;
    const char* description;
};

int main() {
    TestCase tests[] = {
        // Basic cases
        {"PT30S", 30, "30 seconds"},
        {"PT5M", 300, "5 minutes"},
        {"PT2H", 7200, "2 hours"},
        {"P1D", 86400, "1 day"},
        
        // Combined cases
        {"PT1H30M", 5400, "1 hour 30 minutes"},
        {"PT2H30M45S", 9045, "2 hours 30 minutes 45 seconds"},
        {"P1DT2H", 93600, "1 day 2 hours"},
        {"P1DT2H30M", 95400, "1 day 2 hours 30 minutes"},
        {"P1DT2H30M45S", 95445, "1 day 2 hours 30 minutes 45 seconds"},
        
        // Case insensitive
        {"pt30s", 30, "lowercase 30 seconds"},
        {"Pt5M", 300, "mixed case 5 minutes"},
        
        // Edge cases
        {"P0D", 0, "0 days (should fail)"},
        {"PT0S", 0, "0 seconds (should fail)"},
        {"", 0, "empty string"},
        {"P", 0, "just P"},
        {"T30S", 0, "missing P"},
        {"P30S", 0, "seconds without T"},
        {"PTD", 0, "D after T (invalid)"},
        {"P1DT", 86400, "day with T but no time components"},
    };
    
    int passed = 0;
    int failed = 0;
    
    std::cout << "Running ISO 8601 Duration Parser Tests\n";
    std::cout << "======================================\n\n";
    
    for (const auto& test : tests) {
        int result = parseISO8601Duration(test.input);
        bool success = (result == test.expected);
        
        if (success) {
            std::cout << "✓ PASS: " << test.description << "\n";
            std::cout << "  Input: \"" << test.input << "\" => " << result << " seconds\n\n";
            passed++;
        } else {
            std::cout << "✗ FAIL: " << test.description << "\n";
            std::cout << "  Input: \"" << test.input << "\"\n";
            std::cout << "  Expected: " << test.expected << " seconds\n";
            std::cout << "  Got: " << result << " seconds\n\n";
            failed++;
        }
    }
    
    std::cout << "======================================\n";
    std::cout << "Results: " << passed << " passed, " << failed << " failed\n";
    
    return (failed == 0) ? 0 : 1;
}
