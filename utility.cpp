inline int clampData(int min, int data, int max) {
    if(data < min)return min;
    if(data > max)return max;
    return data;
}