# include <stdio.h>
int main() {
  char operator;
  int firstNumber,secondNumber;

  do {
    printf("Enter an operator (+, -, *, /): ");
    scanf(" %c", &operator);
    printf("Enter two operands: ");
    scanf("%d %d", &firstNumber, &secondNumber);
    switch (operator) {
      case '+':
        printf("%d + %d = %d\n",firstNumber, secondNumber, firstNumber + secondNumber);
        break;
      case '-':
        printf("%d - %d = %d\n",firstNumber, secondNumber, firstNumber - secondNumber);
        break;
      case '*':
        printf("%d * %d = %d\n",firstNumber, secondNumber, firstNumber * secondNumber);
        break;
      case '/':
        printf("%d / %d = %d\n",firstNumber, secondNumber, firstNumber / secondNumber); // Divide by zero
        break;
      default:
        printf("Error! operator is not correct");
    }
  } while(1);
  return 0;
}
