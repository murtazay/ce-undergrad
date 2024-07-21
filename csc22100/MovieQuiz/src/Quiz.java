import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowEvent;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Random;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTextArea;
import javax.swing.Timer;
import javax.swing.UIManager;
import javax.swing.plaf.FontUIResource;

public class Quiz {

	private Connection connection;
	private Statement sqlStatement;
	private Question[] questions;
	private int numberOfQuestions;
	private Answer[] answers;
	private int numberOfAnswers;
	private JFrame jframe;
	private Question[] questionsUsed;
	private int currentQuestion;
	private int numberOfQuestionsUsed;
	private JTextArea questionTextArea;
	private int score;
	private int deltaPlusScore;
	private int deltaMinusScore;
	private JLabel scoreLabel;
	private Answer[] answerButtons;
	private JPanel answerPanel;
	private final String[][] leaderBoard = new String[10][2];
	private int numberOfScores;
	public static final int NAME = 0;
	public static final int SCORE = 1;
	private int time;
	private JLabel timeLabel;
	private Timer timer;
	private int[] times;

	public static void main(String[] args) {
		Quiz quiz = new Quiz();
		quiz.startQuiz();
	}

	private void startQuiz() {
		setConnection(validateConnection());
		try {
			setSqlStatement(getConnection().createStatement(
					ResultSet.TYPE_SCROLL_INSENSITIVE,
					ResultSet.CONCUR_READ_ONLY));
		} catch (SQLException e) {
			e.printStackTrace();
		}
		setQuestions(importQuestions());
		setAnswers(importAnswers());

		setJframe(new JFrame("Movie Quiz"));
		getJframe().setResizable(false);
		getJframe().setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		getJframe().setSize(800, 600);
		getJframe().add(makeGUI());
		getJframe().setVisible(true);
		getTimer().start();
	}

	/**
	 * Establish the connection to the database
	 * 
	 * @return
	 */
	private Connection validateConnection() {
		Connection connection = null;
		//Uncomment out line below for jar compile to work
		//String connectionString = "jdbc:hsqldb:res:Trivia; shutdown=true";
		//Comment out line below
		String connectionString = "jdbc:hsqldb:file:Trivia; shutdown=true";
		try {
			connection = DriverManager.getConnection(connectionString);
		} catch (SQLException e) {
			e.printStackTrace();
		}
		return connection;
	}

	private Statement getSqlStatement() {
		return sqlStatement;
	}

	private void setSqlStatement(Statement sqlStatement) {
		this.sqlStatement = sqlStatement;
	}

	public Question[] getQuestions() {
		return questions;
	}

	public void setQuestions(Question[] questions) {
		this.questions = questions;
	}

	public int getNumberOfQuestions() {
		return numberOfQuestions;
	}

	public void setNumberOfQuestions(int numberOfQuestions) {
		this.numberOfQuestions = numberOfQuestions;
	}

	/**
	 * Import all the questions from the database
	 * 
	 * @return
	 */
	public Question[] importQuestions() {
		String questionsCount = "SELECT COUNT(*) FROM QUESTIONS";
		String questionsImport = "SELECT * FROM QUESTIONS";
		try {
			ResultSet resultSet = getSqlStatement()
					.executeQuery(questionsCount);
			resultSet.next();
			setNumberOfQuestions(resultSet.getInt(1));
			resultSet = getSqlStatement().executeQuery(questionsImport);
			Question[] questions = new Question[getNumberOfQuestions()];
			resultSet.next();
			for (int i = 0; i < getNumberOfQuestions(); i++, resultSet.next())
				questions[i] = new Question(resultSet.getString(2),
						resultSet.getInt(3), resultSet.getInt(1));
			return questions;
		} catch (SQLException e) {
			e.printStackTrace();
		}
		return null;
	}

	public Answer[] getAnswers() {
		return answers;
	}

	public void setAnswers(Answer[] answers) {
		this.answers = answers;
	}

	public int getNumberOfAnswers() {
		return numberOfAnswers;
	}

	public void setNumberOfAnswers(int numberOfAnswers) {
		this.numberOfAnswers = numberOfAnswers;
	}

	/**
	 * Import all the answers from the database
	 * 
	 * @return
	 */
	public Answer[] importAnswers() {
		String answersCount = "SELECT COUNT(*) FROM ANSWERS";
		String answersImport = "SELECT * FROM ANSWERS";
		try {
			ResultSet resultSet = getSqlStatement().executeQuery(answersCount);
			resultSet.next();
			setNumberOfAnswers(resultSet.getInt(1));
			resultSet = getSqlStatement().executeQuery(answersImport);
			Answer[] answers = new Answer[getNumberOfAnswers()];
			resultSet.next();
			for (int i = 0; i < getNumberOfAnswers(); i++, resultSet.next())
				answers[i] = new Answer(resultSet.getString(2),
						resultSet.getInt(1), resultSet.getString(3), this);
			return answers;
		} catch (SQLException e) {
			e.printStackTrace();
		}
		return null;
	}

	private void setNumberOfScores(int numberOfScores) {
		this.numberOfScores = numberOfScores;
	}

	public int getNumberOfScores() {
		if (numberOfScores < 10)
			return numberOfScores;
		else
			return 10;
	}

	private int getScoreAtIndex(int index) {
		return Integer.parseInt(leaderBoard[index][SCORE]);
	}

	/**
	 * Import the leaderboard from the database
	 */
	private void importLeaderBoard() {
		String leaderBoardCount = "SELECT COUNT(*) FROM LEADERBOARD";
		String leaderBoardImport = "SELECT * FROM LEADERBOARD ORDER BY SCORE DESC";
		try {
			ResultSet resultSet = getSqlStatement().executeQuery(
					leaderBoardCount);
			resultSet.next();
			setNumberOfScores(resultSet.getInt(1));
			resultSet = getSqlStatement().executeQuery(leaderBoardImport);
			resultSet.next();
			for (int i = 0; i < getNumberOfScores(); i++, resultSet.next()) {
				leaderBoard[i][NAME] = resultSet.getString(1);
				leaderBoard[i][SCORE] = resultSet.getString(2);
			}
		} catch (SQLException e) {
			e.printStackTrace();
		}
	}

	/**
	 * Insert into the leaderboard table of the database
	 * 
	 * @param userName
	 * @param score
	 */
	private void addScoreToLBDB(String userName, int score) {
		try {
			int lastHighestScore = 0;
			for (int i = 0; i < getNumberOfScores(); i++)
				if (getScoreAtIndex(i) > score
						&& getScoreAtIndex(i) > lastHighestScore)
					lastHighestScore = getScoreAtIndex(i);
			if (userName.length() > 20)
				userName = userName.substring(0, 19);
			Statement sqlStatement = connection.createStatement();
			sqlStatement.execute("INSERT INTO LEADERBOARD VALUES('" + userName
					+ "', '" + score + "')");
		} catch (SQLException e) {
			e.printStackTrace();
		}
		importLeaderBoard();
	}

	/**
	 * Display the leaderboard
	 */
	private void displayLeaderBoard() {
		String display = String.format("%8s%-23s%5s%n", "Name", " ", "Score");
		for (int i = 0; i < getNumberOfScores(); i++){
			display += String.format("%2d%s%-25.4s%5s%n", (i + 1), ". " ,leaderBoard[i][NAME].toUpperCase(), leaderBoard[i][SCORE]);
		}
		JOptionPane.showConfirmDialog(getJframe(),
				makeTextArea(display, Color.WHITE), "Highscores",
				JOptionPane.CLOSED_OPTION, JOptionPane.PLAIN_MESSAGE);
	}

	public JFrame getJframe() {
		return jframe;
	}

	public void setJframe(JFrame jframe) {
		this.jframe = jframe;
	}

	public void restartGUI(){
		boolean repeat = true;
		for(Question q: getQuestions())
			q.setUsed(false);
		do{
			String[] options = { "1.Easy", "2.Medium", "3.Hard" };
			String optionPicked = (String) JOptionPane.showInputDialog(getJframe(),
					makeTextArea("Please select an option", Color.WHITE),
					"Main Menu", JOptionPane.QUESTION_MESSAGE, null, options,
					options[0]);
			if (optionPicked != null)
				switch (optionPicked) {
				case "1.Easy": {
					setQuestionsUsed(getXQuestions(4));
					setTimes(new int[4]);
					setDeltaPlusScore(100);
					setDeltaMinusScore(50);
					JOptionPane
					.showMessageDialog(
							getJframe(),
							makeTextArea("There are "
									+ getNumberOfQuestionsUsed()
									+ " questions each worth "
									+ getDeltaPlusScore()
									+ " points for a maximum score of "
									+ getNumberOfQuestionsUsed()
									* getDeltaPlusScore() + " points.",
									Color.WHITE));
					repeat = false;
					break;
				}
				case "2.Medium": {
					setQuestionsUsed(getXQuestions(5));
					setTimes(new int[5]);
					setDeltaPlusScore(85);
					setDeltaMinusScore(85);
					JOptionPane
					.showMessageDialog(
							getJframe(),
							makeTextArea("There are "
									+ getNumberOfQuestionsUsed()
									+ " questions each worth "
									+ getDeltaPlusScore()
									+ " points for a maximum score of "
									+ getNumberOfQuestionsUsed()
									* getDeltaPlusScore() + " points.",
									Color.WHITE));
					repeat = false;
					break;
				}
				case "3.Hard": {
					setQuestionsUsed(getXQuestions(6));
					setTimes(new int[6]);
					setDeltaPlusScore(75);
					setDeltaMinusScore(100);
					JOptionPane
					.showMessageDialog(
							getJframe(),
							makeTextArea("There are "
									+ getNumberOfQuestionsUsed()
									+ " questions each worth "
									+ getDeltaPlusScore()
									+ " points for a maximum score of "
									+ getNumberOfQuestionsUsed()
									* getDeltaPlusScore() + " points.",
									Color.WHITE));
					repeat = false;
					break;
				}
			}
			else {
				int sure = JOptionPane.showConfirmDialog(getJframe(), makeTextArea("Are you sure you want to quit?", Color.WHITE), "Exit Confirmation", JOptionPane.YES_NO_OPTION);
				if(sure == JOptionPane.YES_OPTION){
					JOptionPane.showMessageDialog(getJframe(),
							makeTextArea("Thank you for playing!", Color.WHITE),
							"Goodbye", 0);
					getJframe().dispatchEvent(
							new WindowEvent(getJframe(), WindowEvent.WINDOW_CLOSING));
					repeat = false;
				}
			}
		}while(repeat);
		setCurrentQuestion(0);
		setScore(0);
		setTime(-1);
		updateQuestionTextArea();
		updateScoreLabel();
		updateAnswerButtons();
	}
	/**
	 * Generates all the elements needed for the GUI Interface
	 * 
	 * @return
	 */
	// Returns a JPanel containing every element
	// Most of the private variables are set her
	public JPanel makeGUI() {
		UIManager.put("OptionPane.background", Color.BLACK);
		UIManager.put("Panel.background", Color.BLACK);
		UIManager.put("OptionPane.messageFont", new FontUIResource(new Font(
				"Consolas", Font.PLAIN, 14)));
		boolean repeat = true;
		do{
			String[] options = { "1.Easy", "2.Medium", "3.Hard" };
			String optionPicked = (String) JOptionPane.showInputDialog(getJframe(),
					makeTextArea("Please select an option", Color.WHITE),
					"Main Menu", JOptionPane.QUESTION_MESSAGE, null, options,
					options[0]);
			if (optionPicked != null)
				switch (optionPicked) {
				case "1.Easy": {
					setQuestionsUsed(getXQuestions(4));
					setTimes(new int[4]);
					setDeltaPlusScore(100);
					setDeltaMinusScore(50);
					JOptionPane
					.showMessageDialog(
							getJframe(),
							makeTextArea("There are "
									+ getNumberOfQuestionsUsed()
									+ " questions each worth "
									+ getDeltaPlusScore()
									+ " points for a maximum score of "
									+ getNumberOfQuestionsUsed()
									* getDeltaPlusScore() + " points.",
									Color.WHITE));
					repeat = false;
					break;
				}
				case "2.Medium": {
					setQuestionsUsed(getXQuestions(5));
					setTimes(new int[5]);
					setDeltaPlusScore(85);
					setDeltaMinusScore(85);
					JOptionPane
					.showMessageDialog(
							getJframe(),
							makeTextArea("There are "
									+ getNumberOfQuestionsUsed()
									+ " questions each worth "
									+ getDeltaPlusScore()
									+ " points for a maximum score of "
									+ getNumberOfQuestionsUsed()
									* getDeltaPlusScore() + " points.",
									Color.WHITE));
					repeat = false;
					break;
				}
				case "3.Hard": {
					setQuestionsUsed(getXQuestions(6));
					setTimes(new int[6]);
					setDeltaPlusScore(75);
					setDeltaMinusScore(100);
					JOptionPane
					.showMessageDialog(
							getJframe(),
							makeTextArea("There are "
									+ getNumberOfQuestionsUsed()
									+ " questions each worth "
									+ getDeltaPlusScore()
									+ " points for a maximum score of "
									+ getNumberOfQuestionsUsed()
									* getDeltaPlusScore() + " points.",
									Color.WHITE));
					repeat = false;
					break;
				}
			}
			else {
				int sure = JOptionPane.showConfirmDialog(getJframe(), makeTextArea("Are you sure you want to quit?", Color.WHITE), "Exit Confirmation", JOptionPane.YES_NO_OPTION);
				if(sure == JOptionPane.YES_OPTION){
					JOptionPane.showMessageDialog(getJframe(),
							makeTextArea("Thank you for playing!", Color.WHITE),
							"Goodbye", 0);
					getJframe().dispatchEvent(
							new WindowEvent(getJframe(), WindowEvent.WINDOW_CLOSING));
					repeat = false;
				}
			}
		}while(repeat);
		setCurrentQuestion(0);
		setQuestionTextArea(new JTextArea());
		setScore(0);
		setScoreLabel(new JLabel(Integer.toString(getScore())));

		setTimeLabel(new JLabel());
		setTime(-1);
		setTimer(new Timer(1000, new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent arg0) {
				updateTimeLabel();
				getTimer().restart();
			}

		}));

		JPanel timePanel = new JPanel(new GridLayout(2, 1));
		JLabel time = new JLabel("Time Elapsed(s): ");
		time.setForeground(Color.WHITE);
		timePanel.add(time);
		timePanel.add(getTimeLabel());

		JPanel scorePanel = new JPanel(new GridLayout(1, 2));
		JLabel score = new JLabel("Score: ");
		score.setForeground(Color.WHITE);
		scorePanel.add(score);
		scorePanel.add(getScoreLabel());
		scorePanel.setBackground(Color.BLACK);

		setAnswerButtons(get4Answers());
		setAnswerPanel(new JPanel(new GridLayout(4, 1)));
		for (JButton a : getAnswerButtons())
			getAnswerPanel().add(a);
		JPanel mainPanel = new JPanel(new GridBagLayout());
		mainPanel.setBackground(Color.BLACK);
		GridBagConstraints constraints = new GridBagConstraints();
		constraints.gridx = 1;
		constraints.gridy = 0;
		mainPanel.add(timePanel, constraints);
		constraints.gridx = 2;
		constraints.gridy = 0;
		mainPanel.add(scorePanel, constraints);
		constraints.gridx = 1;
		constraints.gridy = 1;
		mainPanel.add(getQuestionTextArea(), constraints);
		constraints.gridx = 1;
		constraints.gridy = 2;
		mainPanel.add(getAnswerPanel(), constraints);
		return mainPanel;
	}

	/**
	 * Updates the time label
	 */
	protected void updateTimeLabel() {
		setTime(getTime() + 1);
		if (getTime() <= 10)
			getTimeLabel().setForeground(Color.GREEN);
		else if (getTime() <= 20)
			getTimeLabel().setForeground(Color.ORANGE);
		else
			getTimeLabel().setForeground(Color.RED);
		if (getTime() <= 30)
			getTimeLabel().setText(String.format("   %02d", getTime()));
		else
			getTimeLabel().setText("Maximum Points Deducted");
		getTimeLabel().repaint();
	}

	/**
	 * Converts a string into a JTextArea of desired color
	 * 
	 * @param string
	 * @param color
	 * @return
	 */
	public JTextArea makeTextArea(String string, Color color) {
		JTextArea textArea = new JTextArea(string);
		textArea.setEditable(false);
		textArea.setFocusable(false);
		textArea.setBackground(Color.BLACK);
		textArea.setForeground(color.darker());
		if (string.length() < 100)
			textArea.setPreferredSize(new Dimension(300, 80));
		else
			textArea.setPreferredSize(new Dimension(350, 200));
		textArea.setFont(new Font("Consolas", Font.PLAIN, 14));
		textArea.setWrapStyleWord(true);
		textArea.setLineWrap(true);
		return textArea;
	}

	/**
	 * Randomly picks four answers
	 * 
	 * @return
	 */
	public Answer[] get4Answers() {
		Random placer = new Random();
		Answer[] answers = new Answer[4];
		String tag = "";
		for (Answer a : getAnswers())
			if (a.getAnswerId() == getQuestionsUsed()[getCurrentQuestion()]
					.getAnswerId()) {
				a.setActionCommand("Correct");
				answers[placer.nextInt(4)] = a;
				tag += a.getTag();
				break;
			}
		int size = 0;
		for (Answer a : getAnswers())
			if (a.getTag().equals(tag))
				size++;
		Answer[] relatedAnswers = new Answer[size];
		int rAIndex = 0;
		for (Answer a : getAnswers())
			if (a.getTag().equals(tag)) {
				relatedAnswers[rAIndex] = a;
				rAIndex++;
			}
		int filled = 1;
		int aIndex = 0;
		rAIndex = 0;
		while (filled < 4) {
			for (int i = 0; i < placer.nextInt(); i++)
				if (rAIndex >= size - 1)
					rAIndex = 0;
				else
					rAIndex++;
			boolean dupTrue = false;
			for (Answer a : answers)
				if (a != null
				&& a.getAnswerId() == relatedAnswers[rAIndex]
						.getAnswerId()) {
					dupTrue = true;
					break;
				}
			if (answers[aIndex] == null && dupTrue == false) {
				answers[aIndex] = relatedAnswers[rAIndex];
				filled++;
				aIndex++;
			} else if (answers[aIndex] != null && dupTrue == false)
				aIndex++;
		}
		return answers;
	}

	/**
	 * Update the GUI
	 */
	// Most of the private variables are changed to update
	protected void updateGUI() {
		if (getQuestionsUsed().length > getCurrentQuestion() + 1){
			setCurrentQuestion(getCurrentQuestion() + 1);
			updateQuestionTextArea();
			updateScoreLabel();
			updateAnswerButtons();
			setTime(0);
			getTimeLabel().setText(String.format("   %02d", getTime()));
			getTimeLabel().repaint();
			getTimer().restart();
		}
		else {
			int totalTime = 0;
			for (int i : getTimes())
				totalTime += i;
			if(totalTime < 0)
				totalTime = 0;
			JOptionPane.showMessageDialog(
					getJframe(),
					makeTextArea("Final Score: " + getScore()
							+ "\nTotal time: " + totalTime + " seconds",
							Color.WHITE));
			importLeaderBoard();
			if (getNumberOfScores() > 0
					&& getScore() > getScoreAtIndex(getNumberOfScores() - 1)
					|| getNumberOfScores() == 0 && getScore() > 0)
				try {
					addScoreToLBDB(
							JOptionPane.showInputDialog(makeTextArea(
									"You've hit a new high score\n Enter your initials(3 Characters Only):",
									Color.WHITE)), getScore());
				} catch (NullPointerException e) {
					int sure = JOptionPane.showConfirmDialog(getJframe(), makeTextArea("Are you sure you want to quit?", Color.WHITE), "Exit Confirmation", JOptionPane.YES_NO_OPTION);
					if(sure == JOptionPane.YES_OPTION){
						JOptionPane.showMessageDialog(getJframe(),
								makeTextArea("Thank you for playing!", Color.WHITE),
								"Goodbye", 0);
						getJframe().dispatchEvent(
								new WindowEvent(getJframe(),
										WindowEvent.WINDOW_CLOSING));
						try {
							getConnection().close();
						} catch (SQLException e1) {
							e1.printStackTrace();
						}
					}
				}
			displayLeaderBoard();
			int choice = JOptionPane.showConfirmDialog(getJframe(), makeTextArea("Play again?", Color.WHITE), "Start New Game", JOptionPane.YES_NO_OPTION);
			if(choice == JOptionPane.YES_OPTION){
				restartGUI();
			}
			else{
				JOptionPane.showMessageDialog(getJframe(),
						makeTextArea("Thank you for playing!", Color.WHITE),
						"Goodbye", 0);
				getJframe().dispatchEvent(
						new WindowEvent(getJframe(),
								WindowEvent.WINDOW_CLOSING));
				try {
					getConnection().close();
				} catch (SQLException e1) {
					e1.printStackTrace();
				}
			}
		}
	}

	/**
	 * Updates the Screen with "new" Buttons
	 */
	public void updateAnswerButtons() {
		for (Answer a : getAnswerButtons())
			getAnswerPanel().remove(a);
		for (Answer a : getAnswers())
			a.setActionCommand("Incorrect");
		setAnswerButtons(get4Answers());
		for (Answer a : getAnswerButtons())
			getAnswerPanel().add(a);
	}

	/**
	 * Randomly selects i number of questions
	 * 
	 * @param i
	 * @return
	 */
	public Question[] getXQuestions(int i) {
		setNumberOfQuestionsUsed(i);
		Question[] questions = new Question[i];
		Random picker = new Random();
		int chosen = 0;
		int slot = picker.nextInt(getNumberOfQuestions());
		while (chosen < i)
			if (!getQuestions()[slot].isUsed()) {
				getQuestions()[slot].setUsed(true);
				questions[chosen] = getQuestions()[slot];
				chosen++;
				slot = picker.nextInt(getNumberOfQuestions());
			} else
				slot = picker.nextInt(getNumberOfQuestions());
		return questions;
	}

	public Question[] getQuestionsUsed() {
		return questionsUsed;
	}

	private Connection getConnection() {
		return connection;
	}

	private void setConnection(Connection connection) {
		this.connection = connection;
	}

	public void setQuestionsUsed(Question[] questionsUsed) {
		this.questionsUsed = questionsUsed;
	}

	public int getCurrentQuestion() {
		return currentQuestion;
	}

	public void setCurrentQuestion(int currentQuestion) {
		this.currentQuestion = currentQuestion;
	}

	public int getNumberOfQuestionsUsed() {
		return numberOfQuestionsUsed;
	}

	public void setNumberOfQuestionsUsed(int numberOfQuestionsUsed) {
		this.numberOfQuestionsUsed = numberOfQuestionsUsed;
	}

	public JTextArea getQuestionTextArea() {
		return questionTextArea;
	}

	public void setQuestionTextArea(JTextArea questionTextArea) {
		this.questionTextArea = questionTextArea;
		this.questionTextArea.setEditable(false);
		this.questionTextArea.setFocusable(false);
		this.questionTextArea.setBackground(Color.BLACK);
		this.questionTextArea.setForeground(Color.WHITE.darker());
		this.questionTextArea.setText(getQuestionsUsed()[getCurrentQuestion()]
				.getQuestion());
		this.questionTextArea.setPreferredSize(new Dimension(400, getJframe()
				.getHeight() / 3));
		this.questionTextArea.setWrapStyleWord(true);
		this.questionTextArea.setLineWrap(true);
	}

	public void updateQuestionTextArea() {
		getQuestionTextArea().setText(
				getQuestionsUsed()[getCurrentQuestion()].getQuestion());
		getQuestionTextArea().repaint();
	}

	public int getScore() {
		return score;
	}

	public void setScore(int score) {
		this.score = score;
	}

	public JLabel getScoreLabel() {
		return scoreLabel;
	}

	public void setScoreLabel(JLabel scoreLabel) {
		this.scoreLabel = scoreLabel;
		this.scoreLabel.setForeground(Color.WHITE);
	}

	private void updateScoreLabel() {
		getScoreLabel().setText(Integer.toString(getScore()));
		if (getScore() < 0)
			getScoreLabel().setForeground(Color.RED);
		else
			getScoreLabel().setForeground(Color.GREEN);
		getScoreLabel().repaint();
	}

	public Answer[] getAnswerButtons() {
		return answerButtons;
	}

	public void setAnswerButtons(Answer[] answerButtons) {
		this.answerButtons = answerButtons;
	}

	public JPanel getAnswerPanel() {
		return answerPanel;
	}

	public void setAnswerPanel(JPanel answerPanel) {
		this.answerPanel = answerPanel;
	}

	public int getDeltaPlusScore() {
		return deltaPlusScore;
	}

	public void setDeltaPlusScore(int deltaPlusScore) {
		this.deltaPlusScore = deltaPlusScore;
	}

	public int getDeltaMinusScore() {
		return deltaMinusScore;
	}

	public void setDeltaMinusScore(int deltaMinusScore) {
		this.deltaMinusScore = deltaMinusScore;
	}

	public int getTime() {
		return time;
	}

	public void setTime(int time) {
		this.time = time;
	}

	public JLabel getTimeLabel() {
		return timeLabel;
	}

	public void setTimeLabel(JLabel timeLabel) {
		this.timeLabel = timeLabel;
		this.timeLabel.setFont(new FontUIResource(new Font("Consolas",
				Font.PLAIN, 30)));
	}

	public Timer getTimer() {
		return timer;
	}

	public void setTimer(Timer timer) {
		this.timer = timer;
	}

	public int[] getTimes() {
		return times;
	}

	public void setTimes(int[] times) {
		this.times = times;
	}
}
