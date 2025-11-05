#include <bits/stdc++.h>

using namespace std;

vector<int> arr = {176, 480, 395, 109, 289, 478, 132, 70, 229, 70, 278,
    237, 133, 258, 95, 250, 71, 367, 452, 462, 163, 348, 91,
    154, 218, 134, 404, 299, 287, 49, 341, 106, 425, 320, 6,
    147, 130, 79, 414, 105, 419, 499, 26, 60, 371, 322, 460, 480, 323, 130
};

/* Selection Sort.
Time complexity: O(n^2)
Space: O(1) in-place

По сути очень тупой алгоритм, два цикла, и для каждого элемента первого цикла ищем минимум справа, после чего меняем их
*/

void selection_sort(vector<int>& arr) {
    int n = (int)arr.size();
    for(int i = 0; i < n; i++) {
        int min_index = i;
        for(int j = i+1; j < n; j++) {
            if(arr[j] < arr[min_index]) min_index = j;
        }
        if(min_index != i) swap(arr[i], arr[min_index]);
    }

    cout << "Selection sort\n";
}

/* Bubble Sort
Time complexity: O(n^2)
Space: O(1) in-place

Алгоритм еще тупее чем Selection Sort, просто проходим и сравниваем элементы попарно.
В конце каждой итерации внутреннего цикла, 1 элемент встает на свое место
*/

void bubble_sort(vector<int>& arr) {
    int n = (int)arr.size();

    int sorted_index = n;
    while(true) {
        int number_of_swaps = 0;
        for(int i = 0; i < sorted_index-1; i++) {
            if(arr[i] > arr[i+1]) {
                swap(arr[i], arr[i+1]);
                number_of_swaps++;
            }
        }
        sorted_index--;
        if(number_of_swaps == 0) break;
    }

    cout << "Bubble Sort\n";
}

/* Insertion Sort
Time complexity: O(n^2)
Space: O(1)
*/

void insertion_sort(vector<int>& arr) {
    int n = (int)arr.size();

    for(int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i-1;
        while(j >= 0 && arr[j] > key) {
            arr[j+1] = arr[j];
            j--;
        }

        arr[j+1] = key;
    }

    cout << "Insertion Sort\n";
}

/* Merge Sort (Recursive)
Tine complexity: O(n log n)
Space: O(n)
*/

namespace MergeSortRecursive {
void merge(vector<int>& arr, int left, int mid, int right, vector<int>& temp) {
    int i = left, j = mid + 1, k = left;

    // Merge both halves into temp array
    while (i <= mid && j <= right) {
        if(arr[i] <= arr[j]) {
            temp[k] = arr[i];
            i++;
        } else {
            temp[k] = arr[j];
            j++;
        }
        k++;
    }

    // Copy remaining elements
    while(i <= mid) {
        temp[k] = arr[i++];
        k++;
    }

    while(j <= right) {
        temp[k] = arr[j++];
        k++;
    }

    // Copy back to original array
    for(int idx = left; idx <= right; idx++) {
        arr[idx] = temp[idx];
    }
}

void merge_sort_helper(vector<int>& arr, int left, int right, vector<int>& temp) {
    if(left >= right) return;

    int mid = left + (right - left) / 2;

    merge_sort_helper(arr, left, mid, temp);
    merge_sort_helper(arr, mid+1, right, temp);

    merge(arr, left, mid, right, temp);
}

void merge_sort_recursive(vector<int>& arr) {
    if(arr.empty()) return;
    vector<int> temp(arr.size());
    merge_sort_helper(arr, 0, arr.size()-1, temp);
}
} // namespace mergeSort

/* Merge Sort (Iterative)
Time complexity: O(n log n)
Space: O(n)
*/

namespace MergeSortIterative {
void merge(vector<int>& arr, int left, int mid, int right, vector<int>& temp) {
    int i = left, j = mid + 1, k = left;

    while(i <= mid && j <= right) {
        if(arr[i] <= arr[j]) {
            temp[k] = arr[i];
            i++;
        } else {
            temp[k] = arr[j];
            j++;
        }
        k++;
    }

    while(i <= mid) {
        temp[k] = arr[i++];
        k++;
    }

    while(j <= right) {
        temp[k] = arr[j++];
        k++;
    }

    for(int idx = left; idx <= right; idx++) {
        arr[idx] = temp[idx];
    }
}

void merge_sort_iterative(vector<int>& arr) {
    int n = (int)arr.size();
    if (n <= 1) return;

    vector<int> temp(n);

    for(int curSize = 1; curSize < n; curSize *= 2) {
        for(int left = 0; left < n; left += 2 * curSize) {
            int mid = min(left + curSize - 1, n - 1);
            int right = min(left + 2 * curSize - 1, n - 1);

            if(mid < right) {   // only merge if there are two subarrays to merge
                merge(arr, left, mid, right, temp);
            }
        }
    }
}
}// namespace MergeSortIterative

int main(void)
{
    clock_t start = clock();
    ios::sync_with_stdio(0);
    cin.tie(0);


    // selection_sort(arr);
    // bubble_sort(arr);
    // insertion_sort(arr);
    // MergeSortRecursive::merge_sort_recursive(arr);


    for(const auto& el : arr) cout << el << " ";

	cerr << "\nRun Time : " << ((double)(clock() - start) / CLOCKS_PER_SEC) << '\n';

    return 0;
}
