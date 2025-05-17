import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class PrimeChecker extends JFrame {
    private JTextField inputField;
    private JButton checkButton;
    private JProgressBar progressBar;
    private JLabel resultLabel;

    public PrimeChecker() {
        setTitle("Prime Checker");
        setSize(600, 200);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setLocationRelativeTo(null);

        JPanel panel = new JPanel();
        panel.setLayout(new GridLayout(4, 1));

        inputField = new JTextField();
        checkButton = new JButton("Check if Prime");
        progressBar = new JProgressBar(0, 100);
        resultLabel = new JLabel("The result will be shown here.");

        panel.add(inputField);
        panel.add(checkButton);
        panel.add(progressBar);
        panel.add(resultLabel);

        add(panel);

        checkButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                String inputText = inputField.getText().trim();
                try {
                    long number = Long.parseLong(inputText);
                    if (number <= 1) {
                        JOptionPane.showMessageDialog(PrimeChecker.this, "Please enter a number greater than 1.", "Invalid Input", JOptionPane.WARNING_MESSAGE);
                        return;
                    }
                    
                    // 禁用按钮，防止重复点击
                    checkButton.setEnabled(false);
                    checkPrime(number);
                } catch (NumberFormatException ex) {
                    JOptionPane.showMessageDialog(PrimeChecker.this, "Please enter a valid number.", "Invalid Input", JOptionPane.WARNING_MESSAGE);
                }
            }
        });
    }

    private void checkPrime(long number) {
        resultLabel.setText("");
        progressBar.setValue(0);
        Thread workerThread = new Thread(() -> {
            boolean isPrime = true;
            long divisor = -1;
            long maxDivisor = (long) Math.sqrt(number);
            int lastProgress = 0; // 记录上次进度
            for (long i = 2; i <= maxDivisor; i++) {
                if (number % i == 0) {
                    isPrime = false;
                    divisor = i;
                    break;
                }
                int currentProgress = (int) ((i * 100.0) / maxDivisor);
                // 仅当进度变化 ≥1% 时更新
                if (currentProgress > lastProgress) {
                    updateProgress(currentProgress);
                    lastProgress = currentProgress;
                }
            }
            updateProgress(100); // 最终强制更新到 100%
            final long finalDivisor = divisor;
            final boolean finalIsPrime = isPrime;
            SwingUtilities.invokeLater(() -> {
                if (finalIsPrime) {
                    resultLabel.setText(number + " is a prime number.");
                } else {
                    resultLabel.setText(number + " is not a prime number because it is divisible by " + finalDivisor + ".");
                }
                checkButton.setEnabled(true); // 计算完成，重新启用按钮
            });
        });
        workerThread.start();
    }
    private void updateProgress(int progress) {
        SwingUtilities.invokeLater(() -> progressBar.setValue(progress));
    }

    private void updateResult(String text) {
        SwingUtilities.invokeLater(() -> resultLabel.setText(text));
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            PrimeChecker checker = new PrimeChecker();
            checker.setVisible(true);
        });
    }
}