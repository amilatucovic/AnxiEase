# AnxiEase

AnxiEase is a web application designed to help users monitor and manage their stress levels. The app uses a heart rate sensor (**MAX30102**) to measure BPM (beats per minute) in real-time and provides personalized feedback on stress levels. It empowers anxious users to better understand their stressors and take actionable steps toward relaxation and mental well-being.

---

## Features

- **Real-time Heart Rate Monitoring**: Track your pulse with a heart rate sensor and get instant feedback.
- **Stress Level Feedback**: Analyze BPM data to determine your stress level and receive personalized recommendations.
- **Firebase Integration**: Synchronize heart rate data with Firebase for seamless real-time updates.
- **User Guidance**: Clear instructions for sensor usage and actionable feedback.

---

## How It Works

1. **Setup**:
   - Place your finger on the heart rate sensor.
   - The sensor detects your pulse and sends data to the web interface.
2. **Start Measurement**:
   - Click the "Start Measurement" button to begin tracking your heart rate.
   - The app displays your current BPM.
3. **Feedback**:
   - Based on your BPM, the app provides stress-level feedback:
     - Relaxed: Green message.
     - Mild Stress: Orange message.
     - High Stress: Red message.

---

## Technologies Used

- **Frontend**: HTML, CSS (Bootstrap for styling), JavaScript (for real-time interactions)
- **Backend**: Firebase Realtime Database (for storing and retrieving heart rate data)
- **Hardware**:
  - **NodeMCU**: A Wi-Fi-enabled microcontroller for reading sensor data and sending it to Firebase.
  - **MAX30102**: Heart rate sensor for BPM measurement.

---

## Contribution

Contributions are welcome! Feel free to open an issue or submit a pull request.
