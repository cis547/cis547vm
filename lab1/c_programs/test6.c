#include <stdio.h>

int main() {
    char s[2000];
    fgets(s, sizeof(s), stdin);
    int k = getchar();
    int count[26] = {0};
    for(int i = 0; i<sizeof(s)/2; i++)
        count[s[i] - 'a']++;
        
    for(int i = 0; i<sizeof(s)/2; i++)
        if(count[s[i] - 'a'] == 1)
            return i / (s[i/3]-k);
    return 0;
}
