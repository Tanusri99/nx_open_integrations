document.addEventListener('DOMContentLoaded', function () {
    const objectDetectCheckbox = document.getElementById('objectDetect') as HTMLInputElement;
    const colorDetectCheckbox = document.getElementById('colorDetect') as HTMLInputElement;
    const colorInput = document.getElementById('colorInput') as HTMLInputElement;
    const startButton = document.getElementById('startDetection') as HTMLButtonElement;
    const stopButton = document.getElementById('stopDetection') as HTMLButtonElement;

    startButton.addEventListener('click', () => {
        const objectDetect = objectDetectCheckbox.checked;
        const colorDetect = colorDetectCheckbox.checked;
        const color = colorInput.value;
        const nxCamId = "your-nx-cam-id"; // Retrieve the actual camera ID dynamically if needed

        fetch('http://127.0.0.1:5000/start_detection', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                nx_cam_id: nxCamId,
                object_detect: objectDetect,
                color_detect: colorDetect,
                color_input: color
            })
        })
        .then(response => response.json())
        .then(data => console.log(data))
        .catch(error => console.error('Error:', error));
    });

    stopButton.addEventListener('click', () => {
        fetch('http://127.0.0.1:5000/stop_detection', {
            method: 'POST'
        })
        .then(response => response.json())
        .then(data => console.log(data))
        .catch(error => console.error('Error:', error));
    });
});
