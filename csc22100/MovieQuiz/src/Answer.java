import java.awt.Color;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JOptionPane;

public class Answer extends JButton {

	private static final long serialVersionUID = -3710476520353843963L;
	private String answer;
	private int answerId;
	private String tag;

	public Answer(String answer, int answerId, String tag, final Quiz quiz) {
		setAnswer(answer);
		setAnswerId(answerId);
		setTag(tag);
		setBackground(Color.BLACK);
		setForeground(Color.GREEN);
		setActionCommand("Incorrect");
		setPreferredSize(new Dimension(400, 30));
		setText(getAnswer());
		addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				// TODO Auto-generated method stub
				quiz.getTimer().stop();
				quiz.getTimes()[quiz.getCurrentQuestion()] = quiz.getTime();
				int timeBonus = 0;
				if (quiz.getTime() <= 5)
					timeBonus = 10;
				else if (quiz.getTime() <= 10)
					timeBonus = 0;
				else if (quiz.getTime() <= 30)
					timeBonus = -(quiz.getTime() - 5);
				else
					timeBonus = -25;
				if ("Correct".equals(e.getActionCommand())) {
					quiz.setScore(quiz.getScore() + quiz.getDeltaPlusScore()
							+ timeBonus);
					JOptionPane
							.showMessageDialog(
									quiz.getJframe(),
									quiz.makeTextArea(
											e.getActionCommand()
													+ "\nScore +"
													+ quiz.getDeltaPlusScore()
													+ "\tTime Bonus: "
													+ timeBonus
													+ "\nTotal score for this question: +"
													+ (quiz.getDeltaPlusScore() + timeBonus)
													+ "\nThere are "
													+ (quiz.getNumberOfQuestionsUsed()
															- quiz.getCurrentQuestion() - 1)
													+ " questions left.",
											Color.GREEN.brighter()));
					quiz.updateGUI();
				} else if ("Incorrect".equals(e.getActionCommand())) {
					quiz.setScore(quiz.getScore() - quiz.getDeltaMinusScore()
							- timeBonus);
					JOptionPane
							.showMessageDialog(
									quiz.getJframe(),
									quiz.makeTextArea(
											e.getActionCommand()
													+ "\nScore -"
													+ quiz.getDeltaMinusScore()
													+ "\tTime Bonus: "
													+ timeBonus
													+ "\nTotal score for this question: "
													+ (-quiz.getDeltaMinusScore() - timeBonus)
													+ "\nThere are "
													+ (quiz.getNumberOfQuestionsUsed()
															- quiz.getCurrentQuestion() - 1)
													+ " questions left.",
											Color.RED.brighter()));
					quiz.updateGUI();
				}
			}
		});
	}

	public String getAnswer() {
		return answer;
	}

	public void setAnswer(String answer) {
		this.answer = answer;
	}

	public int getAnswerId() {
		return answerId;
	}

	public void setAnswerId(int answerId) {
		this.answerId = answerId;
	}

	public String getTag() {
		return tag;
	}

	public void setTag(String tag) {
		this.tag = tag;
	}

}
