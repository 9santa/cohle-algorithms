
/*
Алгоритм Бойера-Мура (Boyer-Moore Majority Vote Algorithm):

Принцип работы:

Инициализируем candidate и count = 0
Проходим по всем элементам массива:
Если count == 0, выбираем текущий элемент как нового кандидата
Если текущий элемент равен кандидату, увеличиваем счетчик
Если текущий элемент не равен кандидату, уменьшаем счетчик
Возвращаем кандидата

Почему это работает:
Алгоритм основан на том, что majority элемент встречается более n/2 раз
Каждый раз, когда мы видим элемент, отличный от кандидата, мы "нейтрализуем" один голос за кандидата
Так как majority элемент встречается более половины раз, он в конечном итоге останется кандидатом
*/

int majority_element(vector<int>& nums) {
    int count = 0;
    int candidate = 0;

    for (int num : nums) {
        if (count == 0) {
            candidate = num;
        }
        count += (num == candidate) ? 1 : -1;
    }

    return candidate;
}


// Modification of the algorithms for finding elements that appear more than (n/3) times
vector<int> majorityElement(vector<int>& nums) {
    int n = (int)nums.size();
    int candidate1 = 0, candidate2 = 0;
    int count1 = 0, count2 = 0;
    for (int i = 0; i < n; i++) {
        if(nums[i] == candidate1) count1++;
        else if(nums[i] == candidate2) count2++;
        else if(count1 == 0) {
            candidate1 = nums[i];
            count1 = 1;
        }
        else if(count2 == 0) {
            candidate2 = nums[i];
            count2 = 1;
        }
        else {
            count1--;
            count2--;
        }
    }

    count1 = 0, count2 = 0;
    for(int num : nums) {
        if(num == candidate1) count1++;
        else if(num == candidate2) count2++;
    }

    vector<int> result;
    if(count1 > n/3) result.push_back(candidate1);
    if(count2 > n/3) result.push_back(candidate2);

    return result;
}
