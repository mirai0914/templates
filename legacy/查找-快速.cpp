int
quick_select(int l, int r, int k)
{
    if (l == r) return arr[l];
    int i = l - 1, j = r + 1, x = arr[l];
    while (i < j)
    {
        while(arr[--j] > x);
        while(arr[++i] < x);
        if (i < j)
            swap(arr[i], arr[j]);
    }
    int s_l = j - l + 1;

    if (k > s_l) 
        return quick_select(j + 1, r, k - s_l);
    return quick_select(l, j, k);
}