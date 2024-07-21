public class Question {

	private String question;
	private int answerId;
	private int questionId;
	private boolean used;

	public Question(String question, int answerId, int questionId) {
		setQuestion(question);
		setAnswerId(answerId);
		setQuestionId(questionId);
		setUsed(false);
	}

	public String getQuestion() {
		return question;
	}

	public void setQuestion(String question) {
		this.question = question;
	}

	public int getAnswerId() {
		return answerId;
	}

	public void setAnswerId(int answerId) {
		this.answerId = answerId;
	}

	public int getQuestionId() {
		return questionId;
	}

	public void setQuestionId(int questionId) {
		this.questionId = questionId;
	}

	public boolean isUsed() {
		return used;
	}

	public void setUsed(boolean used) {
		this.used = used;
	}

}
