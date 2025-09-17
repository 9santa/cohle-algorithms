#include <bits/stdc++.h>
#include <vector>

using namespace std;

void generate_subsets(const vector<int>& nums) {
    struct State {
        int index;
        vector<int> current;
    };

    stack<State> st;
    st.push({0, {}});

    while(!st.empty()) {
        State state = st.top();
        st.pop();

        if(state.index == (int)nums.size()) {
            cout << "{ ";
            for(int num : state.current) {
                cout << num << " ";
            }
            cout << "}" << "\n";
            continue;
        }

        // don't include current element
        st.push({state.index + 1, state.current});

        // include current element
        state.current.push_back(nums[state.index]);
        st.push({state.index + 1, state.current});
    }
}


int main(void) {

    vector<int> nums = {1,2,3,4,5};
    generate_subsets(nums);

    return 0;
}


