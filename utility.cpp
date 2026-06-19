inline float clampData(float min, float data, float max) {
    if(data < min)return min;
    if(data > max)return max;
    return data;
}